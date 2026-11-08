/**
  *****************************************************************************
  * @file		track.c
  * @brief		对自动追光的逻辑接口的算法实现
  * @author		yzcraft
  * @date		2026-10-09
  * @attention
  *****************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "track.h"
#include "app_mode.h"
#include "bsp_adc.h"
#include "filter.h"
#include "gimbal.h"
#include <math.h>

/* Private define ------------------------------------------------------------*/
/** 滑动平均窗口长度:8 刻 x 10ms = 80ms 平滑 */
#define TRACK_FILTER_WINDOW        8U

/** 相对容差比例:两方向差异小于"环境亮度总量"的 x% 即认为已正对 */
#define TRACK_TOLERANCE_RATIO      0.02f	/* 手机手电筒 0.01-0.02f 30CM正常运行 */
											/* 小卖部没手电筒,暂时测不出来 */

/** 环境亮度总量上限:高于此值视为完全无光(12 位 ADC,四路满值约 16380) */
#define TRACK_TOTAL_LEVEL_MAX      16000.0f

/** 每个周期单轴最大转角(度)的封顶值:1.5度 / 10ms = 150度/s
 *  只有信号很强(离得远,读数大)时才会用到;近距离的实际步长按下面的标定缩小. */
#define TRACK_STEP_LIMIT_DEG       1.5f

/** 步长标定:
 *  总量等于 TRACK_STEP_REF_LEVEL 时,步长取 TRACK_STEP_REF_DEG
 *  实测:该总量下固定 0.3-0.4度 能稳定收敛,固定 0.5度 就会来回摆
 *  步长按总量线性放大(远处读数大,容差带在角度上也更宽),最大 TRACK_STEP_LIMIT_DEG
 *  注意 level_sum 是四路原始读数之和,不是 OLED 上除以 42 的显示值:
 *  显示 20~30 对应原始 840~1260,四路合计约 4200 */
#define TRACK_STEP_REF_LEVEL       4200.0f
#define TRACK_STEP_REF_DEG         0.4f

/** 连续多少刻落在容差内才认定为"已正对":15 刻 x 10ms = 150ms */
#define TRACK_ALIGN_CONFIRM_TICKS  15U

/** 退出"已正对"状态时的计数回退步长,用于抑制边界抖动 */
#define TRACK_ALIGN_RELEASE_STEP   5U

/** 方位轴转向映射:实测方向反了就改符号 */
#define TRACK_YAW_DIRECTION        (+1.0f)

/** 俯仰轴转向映射:实测方向反了就改符号 */
#define TRACK_PITCH_DIRECTION      (+1.0f)
/* Private variables ---------------------------------------------------------*/
static filter_ma_t track_level_filter[TRACK_SENSOR_NUM];  /* 四路各一个滑动平均 */
static uint16_t track_level[TRACK_SENSOR_NUM];            /* 滤波后读数:左 右 上 下 */

static float track_left_right_diff;                       /* 左 - 右,仅用于调试观察 */
static float track_up_down_diff;                          /* 上 - 下,仅用于调试观察 */

static track_state_t track_state;                               /* 当前追光状态 */
static uint8_t track_align_ticks;                         /* 连续落在容差内的刻数 */

/* 两轴浮点目标角:用浮点才能累加 1.5度 这样的小步长 */
static float track_target_yaw_deg;
static float track_target_pitch_deg;

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief	追光的唯一执行出口
 * @param			axis				传入轴LOGIC_GIMBAL_YAW , LOGIC_GIMBAL_PITCH
 * @param[in,out]	*axis_target_deg	传入当前目标角并不断累加更新
 * @param			step_delta_deg		计算出来的步进角度
 * @param			step_limit_deg		限步值
 */
static void track_step_gimbal_axis(logic_gimbal_axis_t axis, float *axis_target_deg,float step_delta_deg, float step_limit_deg);


/* Exported functions --------------------------------------------------------*/

void logic_track_init(void)
{
	for(uint8_t i = 0; i < TRACK_SENSOR_NUM; i++ )
	{
		filter_ma_init(&track_level_filter[i], TRACK_FILTER_WINDOW);
	}

	/* 以云台当前目标角为起点 */
	track_target_pitch_deg = (float)logic_gimbal_get_target_angle(LOGIC_GIMBAL_PITCH);
	track_target_yaw_deg = (float)logic_gimbal_get_target_angle(LOGIC_GIMBAL_YAW);

	track_state = TRACK_STATE_NO_LIGHT;
	track_align_ticks = 0;
	track_left_right_diff = 0.0f;
	track_up_down_diff = 0.0f;
}


void logic_track_reset(void)
{
	/* 清过滤器 */
	for(uint8_t i = 0; i < TRACK_SENSOR_NUM; i++)
	{
		filter_ma_reset(&track_level_filter[i]);
	}
	/* 以云台当前目标角为起点 */
	track_target_pitch_deg = (float)logic_gimbal_get_target_angle(LOGIC_GIMBAL_PITCH);
	track_target_yaw_deg = (float)logic_gimbal_get_target_angle(LOGIC_GIMBAL_YAW);

	track_state = TRACK_STATE_NO_LIGHT;
	track_align_ticks = 0;
	track_left_right_diff = 0.0f;
	track_up_down_diff = 0.0f;
}

void logic_track_task(void)
{
	/* 仅在模式三可用 */
	if(app_mode_get() != MODE_TRACK) {return ;}

	uint16_t raw_level[TRACK_SENSOR_NUM];
	bsp_adc_read(raw_level);
	for (uint8_t i = 0; i < TRACK_SENSOR_NUM; i++ )
	{
		track_level[i] = (uint16_t)filter_ma_update(&track_level_filter[i], (float)raw_level[i]);

	}

	/* 计算相对关系 */
	/* 更改为:用同侧两个光敏电阻的平均值来虚拟出上下左右 */
	float level_left = ((float)track_level[TRACK_INDEX_LT] + (float)track_level[TRACK_INDEX_LB])*0.5f;
	float level_right = ((float)track_level[TRACK_INDEX_RT] + (float)track_level[TRACK_INDEX_RB])*0.5f;
	float level_up = ((float)track_level[TRACK_INDEX_LT] + (float)track_level[TRACK_INDEX_RT])*0.5f;
	float level_down = ((float)track_level[TRACK_INDEX_LB] + (float)track_level[TRACK_INDEX_RB])*0.5f;

	float level_sum = level_left + level_right + level_up + level_down;

	float left_right_diff = level_left - level_right;
	float up_down_diff = level_up - level_down;

	track_left_right_diff = left_right_diff;
	track_up_down_diff =  up_down_diff;

	/* 无光保护 */
	if(level_sum > TRACK_TOTAL_LEVEL_MAX)
	{
		track_state = TRACK_STATE_NO_LIGHT;
		track_align_ticks = 0;
		return ;
	}

	/* 相对容差判断是否已经正对 */
	float diff_allowed = level_sum * TRACK_TOLERANCE_RATIO;

	float left_right_gap = fabsf(track_left_right_diff);
	float up_down_gap = fabsf(track_up_down_diff);

	/* 防止gap在对准的边缘来回横跳 */
	if(left_right_gap <= diff_allowed && up_down_gap <= diff_allowed)
	{
		/* 必须连续TRACK_ALIGN_CONFIRM_TICKS刻落在容差内才能认定已对正 */
		if(track_align_ticks < TRACK_ALIGN_CONFIRM_TICKS)
		{
			track_align_ticks ++;
		}

		track_state = (track_align_ticks < TRACK_ALIGN_CONFIRM_TICKS) ? TRACK_STATE_SEEKING:TRACK_STATE_ALIGNED;
		return ;
	}
	/* 对齐计数的滞后回退  */
	/* 落在容差外需要满足一定的刻数才能认定是偏差的 */
	if (track_align_ticks > 0)
	{
		/* 系数选择2.0,标记有明显的偏差,则将 track_align_ticks 回扣至0 */
		if(left_right_gap > diff_allowed*2.0f  || up_down_gap > diff_allowed*2.0f)
		{
			track_align_ticks = 0;
		}
		else if (track_align_ticks >= TRACK_ALIGN_RELEASE_STEP)
		{
			/* 没有明显的的偏差就小幅度回扣 */
			track_align_ticks -= TRACK_ALIGN_RELEASE_STEP;
		}
		else
		{
			/* 不够TRACK_ALIGN_RELEASE_STEP则兜底归零 */
			track_align_ticks = 0;
		}
	}

	track_state = TRACK_STATE_SEEKING;

	/* 调试时认为追光代码的重头戏在这里 */
	/* 这波贪了应该留到春晚改的 */

	/* 步长按总量标定 近距离小步,远距离大步
	 * 近处读数小,容差带在角度上很窄(约 1 度),必须用小步
	 * 远处读数大,带子在角度上很宽,用大步,保持 150 度每秒的接近速度.
	 * 这是为了防止灯光靠近时乱抖动
	 **/
	float step_mag = TRACK_STEP_REF_DEG * (level_sum / TRACK_STEP_REF_LEVEL);

	if (left_right_gap > diff_allowed)
	{
		track_step_gimbal_axis(LOGIC_GIMBAL_YAW, &track_target_yaw_deg,
				TRACK_YAW_DIRECTION * ((left_right_diff > 0.0f) ? step_mag : -step_mag), /* 符号选择 */
				TRACK_STEP_LIMIT_DEG);
	}
	if (up_down_gap > diff_allowed)
	{
		track_step_gimbal_axis(LOGIC_GIMBAL_PITCH, &track_target_pitch_deg,
				TRACK_PITCH_DIRECTION * ((up_down_diff > 0.0f) ? step_mag : -step_mag),
				TRACK_STEP_LIMIT_DEG);
	}
}

track_state_t logic_track_get_state(void)
{
	return track_state;
}

void logic_track_get_level(uint16_t *buf)
{
	if (buf == 0) {return ;}

	for (uint8_t i= 0 ; i < TRACK_SENSOR_NUM; i++ )
	{
		buf[i] = track_level[i];
	}
}




/* Private functions ---------------------------------------------------------*/

static void track_step_gimbal_axis(logic_gimbal_axis_t axis, float *axis_target_deg,float step_delta_deg, float step_limit_deg)
{
	if (axis_target_deg == 0) {return ;}

	/* 单周期限步 防止冲过头回摆 */
	if(step_delta_deg > step_limit_deg) {step_delta_deg = step_limit_deg;}
	if(step_delta_deg < -step_limit_deg) {step_delta_deg = -step_limit_deg;}

	/* 同步修改目标角 */
	*axis_target_deg += step_delta_deg;

	/* 饱和钳位到机械行程[0,180] */
	if(*axis_target_deg < 0.0f) {*axis_target_deg = 0.0f;}
	if(*axis_target_deg > 180.0f) {*axis_target_deg = 180.0f;}

	/* 浮点累加后 取整并设置舵机角度,并防止无限原地踏步*/
	logic_gimbal_set_angle(axis, (uint8_t)(*axis_target_deg + 0.5f)); /* +0.5f在整数截断机制下相当于四舍五入 */
}
