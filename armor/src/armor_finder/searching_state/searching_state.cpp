#include <armor_finder/armor_finder.h>
#include <show_images/show_images.h>
#include <options.h>
#include <log.h>

bool ArmorFinder::stateSearchingTarget(cv::Mat &src) {
    if (findArmorBox(src, target_box)) { // 在原图中寻找目标，并返回是否找到
        cout<<"Debug: "<<__FUNCTION__ <<__LINE__<<endl;
        if (last_box.rect != cv::Rect2d() &&
            (getPointLength(last_box.getCenter() - target_box.getCenter()) > last_box.rect.height * 2.0) &&
            anti_switch_cnt++ < 10) {   // 判断当前目标和上次有效目标是否为同一个目标
            target_box = ArmorBox();   // 并在未来３帧，在长度为2倍目标装甲板距离范围内试图找到相同目标
            LOGM("anti-switch!");      // 即刚发生目标切换内的３帧内不发送目标位置
            return false;              // 可以一定程度避免频繁多目标切换
        } else {
            anti_switch_cnt = 0;
            return true;
        }
    } else {
        target_box = ArmorBox();
        anti_switch_cnt++;
        return false;
    }
}

/*
bool ArmorFinder::stateSearchingTarget(cv::Mat &src) {
    if (findArmorBox(src, target_box)) {
        return true;
    } else {
        target_box = ArmorBox();
        return false;
    }
}
*/