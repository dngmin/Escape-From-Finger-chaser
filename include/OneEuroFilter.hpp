#pragma once
#include <SFML/Graphics.hpp>

class OneEuroFilter
{
public:
    OneEuroFilter(const float f_c_min, const float b, const float d_cutoff);

    sf::Vector2f Filter(const sf::Vector2f& raw_pos, float dT);

    sf::Vector2f getVelocity() const {return curr_vel;}

private:
    // Playerのデータ
    sf::Vector2f prev_pos; // 過去の位置
    sf::Vector2f curr_vel; // 現在速度
    sf::Vector2f prev_vel; // 過去の速度

    // dTが 0になることを防ぐためのガード
    static constexpr float MIN_dT = 1e-6f;

    // f_c_min : 最小遮断周波数
    // 今後alpha計算の際、分母となるため0は不可
    const float min_cutoff_frequency = 1.f;
    // 速度係数(又は速度敏感度)
    // 大きくすると、急変化時にも反応が早いが、ノイズはより入る
    // 小さくすると、ノイズが少なく滑らかな軌跡を見せるが、遅延が生じる
    const float beta = 0.01f;
    // d_cutoff；微分値の遮断周波数
    // 今後alpha計算の際、分母となるため0は不可
    const float derivative_cutoff    = 1.f;

    // 2 * pi
    static constexpr float two_pi = 6.2831853f;

    bool is_first = true;

    // LPF : Low Pass Filter計算
    float solve_LPF(float curr, float prev, float alpha) const
    {
        return alpha * curr + (1.f - alpha) * prev;
    }

    // LPFに使われる フィルター係数(又は平滑化係数)取得
    float get_alpha(float dT, float cutoff) const
    {
        return 1.f / (1.f + 1.f / (two_pi * dT * cutoff));
    }
};