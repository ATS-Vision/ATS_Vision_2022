#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#define PI (3.14159265459)

#define ENEMY_BLUE 0
#define ENEMY_RED  1

#define ALLY_BLUE ENEMY_RED
#define ALLY_RED  ENEMY_BLUE

#define BIG_ENERGY_STATE   'b'
#define SMALL_ENERGY_STATE 's'
#define ARMOR_STATE        'a'

#define FOCUS_PIXAL_8MM  (1488)
#define FOCUS_PIXAL_5MM  (1280) // TODO:需要补充更多镜头焦段的测试
#define FOCUS_PIXAL_6MM  ()
#define FOCUS_PIXAL      FOCUS_PIXAL_5MM

#endif /* _CONSTANTS_H */
