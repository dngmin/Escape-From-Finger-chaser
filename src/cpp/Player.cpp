#include "Player.hpp"
#include <cmath>

// Playerのデータを更新
// 現在位置生データから1€ filterを用いて補正
// 既存のLow Pass Filterでの固定されたフィルター係数を可変的にした手法
void PlayerState::update(sf::Vector2f received_pos)
{
    // 1€ filterの実装
    // dTを測定
    TimePoint T = Clock::now();
    dT = std::chrono::duration<double>(T - T_prev).count();
    // dT < MIN_dTになるとMIN_DTを使用する
    // 次のcurr_velを求めるときZeroDivisionErrorを防ぐため
    dT = (dT > MIN_dT? dT : MIN_dT);

    // 位置変化なし又は変化が微小すぎる場合はreturn
    // 長時間止まっていてまた動き出してもdTが膨大な数字になることを防ぐため、dT計算より後に配置
    // 更新できずreturnされたらchasersを騙すフェイントが仕掛ける。修正要
    if (received_pos == curr_pos)
    {
        // 時間は更新するが、止まっているとみなし、速度を0にする
        T_prev = T;
        prev_pos = curr_pos;
        curr_vel = {0.f, 0.f};
        prev_vel = {0.f, 0.f};
        return;
    }

    // 変移から速度を逆算する
    curr_vel.x = (received_pos.x - prev_pos.x) / dT;
    curr_vel.y = (received_pos.y - prev_pos.y) / dT;

    // filtering velocity
    // 一次的に速度filteringを行うためのalpha取得
    double alpha_derivative = get_alpha(dT, derivative_cutoff);
    // 一次的に速度をfiltering
    curr_vel.x = solve_LPF(curr_vel.x, prev_vel.x, alpha_derivative);
    curr_vel.y = solve_LPF(curr_vel.y, prev_vel.y, alpha_derivative);

    // adapted alpha
    // 位置補正に用いるalphaを取得
    double cutoff_frequency_x = min_cutoff_frequency + beta * std::abs(curr_vel.x);
    double cutoff_frequency_y = min_cutoff_frequency + beta * std::abs(curr_vel.y);
    double alpha_x = get_alpha(dT, cutoff_frequency_x);
    double alpha_y = get_alpha(dT, cutoff_frequency_y);

    // filtering
    // alphaを用いて速度を補正
    curr_pos.x = solve_LPF(received_pos.x, prev_pos.x, alpha_x);
    curr_pos.y = solve_LPF(received_pos.y, prev_pos.y, alpha_y);

    // 次の計算のため値変換
    T_prev = T;
    curr_acc = curr_vel - prev_vel;
    prev_vel = curr_vel;
    prev_pos = curr_pos;
}

// 現在位置取得
sf::Vector2f PlayerState::get_curr_pos()
{
    return curr_pos;
}

// Euler法
// x_(t+1) = x_t + dx_t * dt
// 予測位置 = 現在位置 + 現在速度 * 経過時間(ここでは 予測ステップ数 * dt)
sf::Vector2f PlayerState::get_Euler_predict()
{
    float predict_x = curr_pos.x + curr_vel.x * (prediction_steps * dT);
    float predict_y = curr_pos.y + curr_vel.y * (prediction_steps * dT);
    return {predict_x, predict_y};
}

// LPF : Low Pass Filter
// 補正後の現在値 = フィルター係数 * 生の現在値 + (1 - alpha) * 過去値(前回の補正後の現在値)
double PlayerState::solve_LPF(double curr, double prev, double alpha)
{
    return alpha * curr + (1 - alpha) * prev;
}

// フィルター係数(又は平滑化係数)
// 1に近いほどフィルターは弱い : 反応が早くなるが、ノイズがあまり消えない
// 0に近いほどフィルターは強い : ノイズが強力に除去できるが、反応が遅れる
double PlayerState::get_alpha(double dT, double cutoff)
{
    return 1 / (1 + 1 / (two_pi * dT * cutoff));
}