#pragma once

class Constants
{
public:
	static constexpr const char* prefix = "start_position:";
	static constexpr size_t prefix_len = sizeof("start_position:") - 1;
	static constexpr const char* player_prefix = "player:";
	static constexpr size_t player_prefix_len = sizeof("player:") - 1;
	static constexpr const char* enemy_prefix = "enemy:";
	static constexpr size_t enemy_prefix_len = sizeof("enemy:") - 1;
};