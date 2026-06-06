#include "OneEuroFilter.hpp"
#include <cmath>

OneEuroFilter::OneEuroFilter(const float f_c_min, const float b, const float d_cutoff)
    : min_cutoff_frequency(f_c_min)
    , beta(b)
    , derivative_cutoff(d_cutoff)
{}

sf::Vector2f OneEuroFilter::Filter(const sf::Vector2f& raw_pos, float dT)
{
    // 初期化
    if (is_first)
    {
        prev_pos = raw_pos;
        curr_vel = {0.f, 0.f};
        prev_vel = {0.f, 0.f};
        is_first = false;
        return raw_pos;
    }

    // 次のcurr_velを求めるときZeroDivisionErrorを防ぐため
    dT = (dT > MIN_dT? dT : MIN_dT);

    // 現在このままでは生データをそのまま返すため、補正されい問題がある。そのため現在は未使用とする。
    // 位置変化なし又は変化が微小すぎる場合はreturn
    // 長時間止まっていてまた動き出してもdTが膨大な数字になることを防ぐため、dT計算より後に配置
    // if (raw_pos == prev_pos)
    // {
    //     // 時間は更新するが、止まっているとみなし、速度を0にする
    //     curr_vel = {0.f, 0.f};
    //     prev_vel = {0.f, 0.f};
    //     return prev_pos;
    // }

    sf::Vector2f filtered_pos;

    curr_vel.x = (raw_pos.x - prev_pos.x) / dT;
    curr_vel.y = (raw_pos.y - prev_pos.y) / dT;

    // filtering velocity
    float alpha_derivative = get_alpha(dT, derivative_cutoff);
    curr_vel.x = solve_LPF(curr_vel.x, prev_vel.x, alpha_derivative);
    curr_vel.y = solve_LPF(curr_vel.y, prev_vel.y, alpha_derivative);

    // adapted alpha
    float cutoff_frequency_x = min_cutoff_frequency + beta * std::abs(curr_vel.x);
    float cutoff_frequency_y = min_cutoff_frequency + beta * std::abs(curr_vel.y);
    float alpha_x = get_alpha(dT, cutoff_frequency_x);
    float alpha_y = get_alpha(dT, cutoff_frequency_y);

    // filtering
    filtered_pos.x = solve_LPF(raw_pos.x, prev_pos.x, alpha_x);
    filtered_pos.y = solve_LPF(raw_pos.y, prev_pos.y, alpha_y);

    // 次の計算のため値変換
    prev_vel = curr_vel;
    prev_pos = filtered_pos;

    return filtered_pos;
}