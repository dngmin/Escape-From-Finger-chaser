#pragma once
#include <chrono>
#include <SFML/Graphics.hpp>

class PlayerState
{
    // timestampに使用するchronoを短縮定義
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

public:
    // 1€ filterを用いたPlayerのデータアップデート
    void update(sf::Vector2f received_pos);

    // Playerの現在位置を取得
    sf::Vector2f get_curr_pos() const;

    // Euler法を用いたPlayerの予測位置取得
    sf::Vector2f get_Euler_predict() const;

private:
    // Playerのデータ
    sf::Vector2f curr_pos = {0.5f, 0.5f}; // 現在位置
    sf::Vector2f prev_pos = {0.f, 0.f}; // 過去の位置
    sf::Vector2f curr_vel = {0.f, 0.f}; // 現在速度
    sf::Vector2f prev_vel = {0.f, 0.f}; // 過去の速度

    // 基準とする時間(初回呼び出し時に初期化され、update関数より更新される)
    TimePoint T_prev = Clock::now();
    double dT;
    // dTが 0になることを防ぐためのガード
    static constexpr double MIN_dT = 1e-6;

    // 予測ステップ数
    static constexpr int prediction_steps = 5;

    // 1€ filter 初期値。tuningの際には以下を調整

    // f_c_min : 最小遮断周波数
    // 今後alpha計算の際、分母となるため0は不可
    static constexpr double min_cutoff_frequency = 1.0;
    // 速度係数(又は速度敏感度)
    // 大きくすると、急変化時にも反応が早いが、ノイズはより入る
    // 小さくすると、ノイズが少なく滑らかな軌跡を見せるが、遅延が生じる
    static constexpr double beta = 0.01;
    // d_cutoff；微分値の遮断周波数
    // 今後alpha計算の際、分母となるため0は不可
    static constexpr double derivative_cutoff    = 1.0;

    // 2 * pi
    static constexpr double two_pi = 6.2831853071795865;

    // LPF : Low Pass Filter計算
    double solve_LPF(double curr, double prev, double alpha) const;

    // LPFに使われる フィルター係数(又は平滑化係数)取得
    double get_alpha(double dT, double cutoff) const;
};