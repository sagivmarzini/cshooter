#include "game.h"

#include "camera.h"

void game_context_init(GameContext* game_context) {
	tile_textures_load(&game_context->tile_textures);
	map_generate_city(&game_context->world.city);
	player_init(&game_context->player, "../assets/cop.png", &game_context->world.city);
	enemy_manager_init(&game_context->enemy_manager, "../assets/thug.png");
	bullet_manager_init(&game_context->bullet_manager, "../assets/bullet.png");
	camera_init();
}

void game_context_unload(GameContext* game_context) {
	tile_textures_unload(&game_context->tile_textures);
	player_unload(&game_context->player);
	enemy_manager_unload(&game_context->enemy_manager);
	bullet_manager_unload(&game_context->bullet_manager);
}
