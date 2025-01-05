#include "imgui/bgfx/imgui_impl_bgfx.h"
#include "imgui/sdl2/imgui_impl_sdl2.h"
#include "viewport/screen.h"

// third-party dependencies
#include <SDL.h>
#include <SDL_syswm.h>
#include <as-ops.h>
#include <bgfx/c99/bgfx.h>
#include <minimal-cmake-gol/gol.h>
#include <minimal-cmake/draw/pos-color-line.h>
#include <minimal-cmake/draw/pos-color-quad.h>
#include <minimal-cmake/draw/pos-color-vertex.h>

#if defined(Status)
#undef Status
#endif // defined(Status)

#include <imgui_te_engine.h>
#include <imgui_te_exporters.h>
#include <imgui_te_ui.h>

// system includes
#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>

extern void RegisterGolTests(ImGuiTestEngine* engine, mc_gol_board_t* board);

static std::vector<char> read_file(const char* filepath) {
  FILE* file = fopen(filepath, "rb");
  if (file == nullptr) {
    return std::vector<char>{};
  }
  // seek to end of file
  if (fseek(file, 0, SEEK_END) != 0) {
    fclose(file);
    return std::vector<char>{};
  }
  // find length of file
  const int64_t file_size = ftell(file);
  if (file_size == -1) {
    fclose(file);
    return std::vector<char>{};
  }
  // return to beginning of file
  if (fseek(file, 0, SEEK_SET) != 0) {
    fclose(file);
    return std::vector<char>{};
  }
  // allocate buffer to hold contents of file
  std::vector<char> buffer(file_size);
  // read file
  const size_t bytes_read = fread(buffer.data(), sizeof(char), file_size, file);
  if (bytes_read == 0) {
    fclose(file);
    return std::vector<char>{};
  }
  fclose(file);
  // return buffer
  return buffer;
}

static bgfx_shader_handle_t create_shader(
  const char* shader, const int size, const char* name) {
  const bgfx_memory_t* mem = bgfx_copy(shader, size);
  const bgfx_shader_handle_t handle = bgfx_create_shader(mem);
  bgfx_set_shader_name(handle, name, static_cast<int>(strlen(name)));
  return handle;
}

double seconds_elapsed(
  const uint64_t previous_counter, const uint64_t current_counter) {
  return static_cast<double>(current_counter - previous_counter)
       / static_cast<double>(SDL_GetPerformanceFrequency());
}

void clear_board(mc_gol_board_t* board) {
  for (int32_t y = 0, board_height = mc_gol_board_height(board);
       y < board_height; y++) {
    for (int32_t x = 0, board_width = mc_gol_board_width(board);
         x < board_width; x++) {
      mc_gol_set_board_cell(board, x, y, false);
    }
  }
}

void reset_board(mc_gol_board_t* board) {
  // gosper glider gun
  mc_gol_set_board_cell(board, 2, 5, true);
  mc_gol_set_board_cell(board, 2, 6, true);
  mc_gol_set_board_cell(board, 3, 5, true);
  mc_gol_set_board_cell(board, 3, 6, true);

  mc_gol_set_board_cell(board, 12, 5, true);
  mc_gol_set_board_cell(board, 12, 6, true);
  mc_gol_set_board_cell(board, 12, 7, true);
  mc_gol_set_board_cell(board, 13, 4, true);
  mc_gol_set_board_cell(board, 13, 8, true);
  mc_gol_set_board_cell(board, 14, 3, true);
  mc_gol_set_board_cell(board, 14, 9, true);
  mc_gol_set_board_cell(board, 15, 3, true);
  mc_gol_set_board_cell(board, 15, 9, true);
  mc_gol_set_board_cell(board, 16, 6, true);
  mc_gol_set_board_cell(board, 17, 4, true);
  mc_gol_set_board_cell(board, 17, 8, true);
  mc_gol_set_board_cell(board, 18, 5, true);
  mc_gol_set_board_cell(board, 18, 6, true);
  mc_gol_set_board_cell(board, 18, 7, true);
  mc_gol_set_board_cell(board, 19, 6, true);

  mc_gol_set_board_cell(board, 22, 3, true);
  mc_gol_set_board_cell(board, 22, 4, true);
  mc_gol_set_board_cell(board, 22, 5, true);
  mc_gol_set_board_cell(board, 23, 3, true);
  mc_gol_set_board_cell(board, 23, 4, true);
  mc_gol_set_board_cell(board, 23, 5, true);
  mc_gol_set_board_cell(board, 24, 2, true);
  mc_gol_set_board_cell(board, 24, 6, true);
  mc_gol_set_board_cell(board, 26, 1, true);
  mc_gol_set_board_cell(board, 26, 2, true);
  mc_gol_set_board_cell(board, 26, 6, true);
  mc_gol_set_board_cell(board, 26, 7, true);

  mc_gol_set_board_cell(board, 36, 3, true);
  mc_gol_set_board_cell(board, 36, 4, true);
  mc_gol_set_board_cell(board, 37, 3, true);
  mc_gol_set_board_cell(board, 37, 4, true);

  // eater
  mc_gol_set_board_cell(board, 27, 20, true);
  mc_gol_set_board_cell(board, 27, 21, true);
  mc_gol_set_board_cell(board, 28, 20, true);
  mc_gol_set_board_cell(board, 28, 21, true);

  mc_gol_set_board_cell(board, 32, 21, true);
  mc_gol_set_board_cell(board, 31, 22, true);
  mc_gol_set_board_cell(board, 33, 22, true);
  mc_gol_set_board_cell(board, 32, 23, true);

  mc_gol_set_board_cell(board, 34, 23, true);
  mc_gol_set_board_cell(board, 34, 24, true);
  mc_gol_set_board_cell(board, 34, 25, true);
  mc_gol_set_board_cell(board, 35, 25, true);
}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(
      stderr, "SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  const as_vec2i screen_dimensions = as_vec2i{.x = 800, .y = 600};
  SDL_Window* window = SDL_CreateWindow(
    argv[0], SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    screen_dimensions.x, screen_dimensions.y, SDL_WINDOW_SHOWN);

  if (window == nullptr) {
    fprintf(
      stderr, "Window could not be created. SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_SysWMinfo wmi;
  SDL_VERSION(&wmi.version);
  if (!SDL_GetWindowWMInfo(window, &wmi)) {
    fprintf(
      stderr, "SDL_SysWMinfo could not be retrieved. SDL_Error: %s\n",
      SDL_GetError());
    return 1;
  }
  bgfx_render_frame(-1); // single threaded mode

  bgfx_renderer_type_t renderer_type = BGFX_RENDERER_TYPE_COUNT;
  bgfx_platform_data_t pd = {};
#if BX_PLATFORM_WINDOWS
  pd.nwh = wmi.info.win.window;
  renderer_type = BGFX_RENDERER_TYPE_DIRECT3D11;
#elif BX_PLATFORM_OSX
  pd.nwh = wmi.info.cocoa.window;
  renderer_type = BGFX_RENDERER_TYPE_METAL;
#elif BX_PLATFORM_LINUX
  pd.ndt = wmi.info.x11.display;
  pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
  renderer_type = BGFX_RENDERER_TYPE_OPENGL;
#endif

  bgfx_init_t bgfx;
  bgfx_init_ctor(&bgfx);
  bgfx.type = renderer_type;
  bgfx.resolution.width = screen_dimensions.x;
  bgfx.resolution.height = screen_dimensions.y;
  bgfx.resolution.reset = BGFX_RESET_VSYNC;
  bgfx.platformData = pd;
  bgfx_init(&bgfx);

  ImGui::CreateContext();
  ImGui_Implbgfx_Init(255);
#if BX_PLATFORM_WINDOWS
  ImGui_ImplSDL2_InitForD3D(window);
#elif BX_PLATFORM_OSX
  ImGui_ImplSDL2_InitForMetal(window);
#elif BX_PLATFORM_LINUX
  ImGui_ImplSDL2_InitForOpenGL(window, nullptr);
#endif // BX_PLATFORM_WINDOWS ? BX_PLATFORM_OSX ? BX_PLATFORM_LINUX

  ImGuiTestEngine* engine = ImGuiTestEngine_CreateContext();
  ImGuiTestEngineIO& test_io = ImGuiTestEngine_GetIO(engine);
  test_io.ConfigVerboseLevel = ImGuiTestVerboseLevel_Info;
  test_io.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Debug;
  test_io.ConfigRunSpeed = ImGuiTestRunSpeed_Normal;

  // start test engine
  ImGuiTestEngine_Start(engine, ImGui::GetCurrentContext());
  ImGuiTestEngine_InstallDefaultCrashHandler();

  mc_gol_board_t* board = mc_gol_create_board(40, 27);
  reset_board(board);

#ifdef MC_GOL_APP_BUILD_TESTING
  // register tests
  RegisterGolTests(engine, board);
#ifndef MC_GOL_APP_INTERACTIVE_TESTING
  // queue tests
  ImGuiTestEngine_QueueTests(
    engine, ImGuiTestGroup_Tests, "gol-tests", ImGuiTestRunFlags_RunFromGui);
#endif // MC_GOL_APP_INTERACTIVE_TESTING
#endif // MC_GOL_APP_BUILD_TESTING

  const bgfx_vertex_layout_t pos_col_vert_layout =
    create_pos_col_vert_layout(renderer_type);

  char* base_path = SDL_GetBasePath();

  std::string vs_shader_full_path;
  vs_shader_full_path.append(base_path);
  vs_shader_full_path.append("shader/vs_vertcol.bin");

  std::string fs_shader_full_path;
  fs_shader_full_path.append(base_path);
  fs_shader_full_path.append("shader/fs_vertcol.bin");

  std::vector<char> vs_shader = read_file(vs_shader_full_path.c_str());
  std::vector<char> fs_shader = read_file(fs_shader_full_path.c_str());
  if (vs_shader.empty() || fs_shader.empty()) {
    fprintf(
      stderr, "Shaders not found. Have you built them using "
              "compile-shader-<platform>.sh/bat script?\n");
    return 1;
  }

  const bgfx_shader_handle_t vertex_shader = create_shader(
    vs_shader.data(), static_cast<int>(vs_shader.size()), "vs_shader");
  const bgfx_shader_handle_t fragment_shader = create_shader(
    fs_shader.data(), static_cast<int>(fs_shader.size()), "fs_shader");
  const bgfx_program_handle_t program =
    bgfx_create_program(vertex_shader, fragment_shader, true);

  // deallocate buffers
  std::vector<char>().swap(vs_shader);
  std::vector<char>().swap(fs_shader);
  // deallocate base path
  SDL_free(base_path);

  const bgfx_uniform_handle_t u_color =
    bgfx_create_uniform("u_color", BGFX_UNIFORM_TYPE_VEC4, 1);

  pos_color_lines_t* pos_color_lines = create_pos_color_lines();
  pos_color_lines_set_render_context(
    pos_color_lines, 0, program, &pos_col_vert_layout, u_color);

  pos_color_quads_t* pos_color_quads =
    create_pos_color_quads(&pos_col_vert_layout);
  pos_color_quads_set_render_context(pos_color_quads, 0, program, u_color);

  bool simulating = true;
  const float zoom = 20.0f;
  const as_mat44f identity = as_mat44f_identity();
  const float aspect_ratio = static_cast<float>(screen_dimensions.x)
                           / static_cast<float>(screen_dimensions.y);
  const as_mat44f orthographic_projection = as_mat44f_transpose_v(
    as_mat44f_orthographic_projection_depth_zero_to_one_lh(
      -zoom * aspect_ratio, zoom * aspect_ratio, -zoom, zoom, 0.0f, 1.0f));
  bgfx_set_view_transform(0, identity.elem, orthographic_projection.elem);

  const float board_width = static_cast<float>(mc_gol_board_width(board));
  const float board_height = static_cast<float>(mc_gol_board_height(board));
  const as_vec3f board_top_left_cell_center = as_vec3f{
    .x = (-board_width * 0.5f) + 0.5f, .y = (board_height * 0.5f) - 0.5f};

  int exit_code = 0;
  float delay = 0.1f;
  double timer = 0.0;
  double previous_frame_time = SDL_GetPerformanceFrequency();
  for (bool running = true; running;) {
    for (SDL_Event current_event; SDL_PollEvent(&current_event) != 0;) {
      ImGui_ImplSDL2_ProcessEvent(&current_event);
      if (current_event.type == SDL_QUIT) {
        running = false;
        break;
      }
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
      const ImVec2 mouse_now = ImGui::GetMousePos();
      const as_point3f position = world_from_screen(
        as_point2i_from_point2f(
          {mouse_now.x, screen_dimensions.y - mouse_now.y}),
        &orthographic_projection, screen_dimensions);
      for (int32_t y = 0; y < board_height; y++) {
        for (int32_t x = 0; x < board_width; x++) {
          const as_vec3f cell_top_left_corner = as_vec3f_sub_vec3f(
            as_vec3f_add_vec3f(
              board_top_left_cell_center,
              as_vec3f{
                .x = static_cast<float>(x),
                .y = static_cast<float>(-y),
                .z = 0.5f}),
            as_vec3f{.x = 0.5f, .y = -0.5f});
          if (
            position.x > cell_top_left_corner.x
            && position.x <= cell_top_left_corner.x + 1.0f
            && position.y < cell_top_left_corner.y
            && position.y >= cell_top_left_corner.y - 1.0f) {
            mc_gol_set_board_cell(board, x, y, !mc_gol_board_cell(board, x, y));
          }
        }
      }
    }

    const auto step_board = [board, &timer] {
      mc_gol_update_board(board);
      timer = 0.0;
    };

    ImGui_Implbgfx_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Game of Life");
    ImGui::PushItemWidth(100.0f);
    ImGui::SliderFloat(
      "Simulation time", &delay, 0.01f, 1.0f, "%.2f",
      ImGuiSliderFlags_AlwaysClamp);
    ImGui::PopItemWidth();
    if (ImGui::Button(simulating ? "Pause" : "Play")) {
      timer = 0.0;
      simulating = !simulating;
    }
    if (simulating) {
      ImGui::BeginDisabled();
    }
    ImGui::SameLine();
    if (ImGui::Button("Step")) {
      step_board();
    }
    if (simulating) {
      ImGui::EndDisabled();
    }
    if (ImGui::Button("Clear")) {
      clear_board(board);
      simulating = false;
    }
    if (ImGui::Button("Restart")) {
      clear_board(board);
      reset_board(board);
    }
    ImGui::End();

#ifdef MC_GOL_APP_BUILD_TESTING
#ifdef MC_GOL_APP_INTERACTIVE_TESTING
    ImGuiTestEngine_ShowTestEngineWindows(engine, NULL);
#else
    if (ImGuiTestEngine_IsTestQueueEmpty(engine)) {
      int count_tested = 0;
      int count_success = 0;
      ImGuiTestEngine_GetResult(engine, count_tested, count_success);
      ImGuiTestEngine_PrintResultSummary(engine);
      if (count_tested != count_success) {
        exit_code = 1;
      }
      running = false;
    }
#endif // MC_GOL_APP_INTERACTIVE_TESTING
#endif // MC_GOL_APP_BUILD_TESTING

    const int64_t current_counter = SDL_GetPerformanceCounter();
    const double delta_time =
      seconds_elapsed(previous_frame_time, current_counter);
    previous_frame_time = current_counter;

    timer += delta_time;

    // clear screen
    bgfx_set_view_clear(
      0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xf2f2f2ff, 1.0f, 0);
    bgfx_set_view_rect(0, 0, 0, screen_dimensions.x, screen_dimensions.y);

    const uint32_t line_color = 0xff713d27;
    // horizontal lines
    for (int32_t y = 0; y <= board_height; ++y) {
      const as_vec3f board_top_left_cell_corner =
        as_vec3f{.x = -board_width * 0.5f, .y = board_height * 0.5f};
      pos_color_lines_add_line(
        pos_color_lines,
        pos_color_line_t{
          .begin =
            pos_color_vertex_t{
              .pos = as_vec3f_sub_vec3f(
                board_top_left_cell_corner,
                as_vec3f_mul_float(as_vec3f{.y = 1.0f}, static_cast<float>(y))),
              .abgr = line_color},
          .end = pos_color_vertex_t{
            .pos = as_vec3f_add_vec3f(
              as_vec3f_sub_vec3f(
                board_top_left_cell_corner,
                as_vec3f_mul_float(as_vec3f{.y = 1.0f}, static_cast<float>(y))),
              as_vec3f{board_width, 0.0f, 0.0f}),
            .abgr = line_color}});
    }

    // vertical lines
    for (int32_t x = 0; x <= board_width; ++x) {
      const as_vec3f board_top_left_cell_corner =
        as_vec3f{.x = -board_width * 0.5f, .y = board_height * 0.5f};
      pos_color_lines_add_line(
        pos_color_lines,
        pos_color_line_t{
          .begin =
            pos_color_vertex_t{
              .pos = as_vec3f_add_vec3f(
                board_top_left_cell_corner,
                as_vec3f_mul_float(as_vec3f{.x = 1.0f}, static_cast<float>(x))),
              .abgr = line_color},
          .end = pos_color_vertex_t{
            .pos = as_vec3f_add_vec3f(
              as_vec3f_add_vec3f(
                board_top_left_cell_corner,
                as_vec3f_mul_float(as_vec3f{.x = 1.0f}, static_cast<float>(x))),
              as_vec3f{0.0f, -board_height, 0.0f}),
            .abgr = line_color}});
    }

    // cells
    for (int32_t y = 0; y < board_height; y++) {
      for (int32_t x = 0; x < board_width; x++) {
        const color4f_t cell_color =
          mc_gol_board_cell(board, x, y)
            ? color4f_t{.r = 0.95f, .g = 0.71f, .b = 0.41f, .a = 1.0f}
            : color4f_t{.r = 0.33f, .g = 0.48f, .b = 0.67f, .a = 1.0f};
        const as_vec3f position = as_vec3f_add_vec3f(
          board_top_left_cell_center, as_vec3f{
                                        .x = static_cast<float>(x),
                                        .y = static_cast<float>(-y),
                                        .z = 0.5f});
        pos_color_quads_add_quad(
          pos_color_quads,
          pos_color_quad_t{.position = position, .color = cell_color});
      }
    }

    if (simulating && timer > delay) {
      step_board();
    }

    pos_color_lines_submit(pos_color_lines);
    pos_color_quads_submit(pos_color_quads);

    ImGui::Render();
    ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());

    bgfx_touch(0);
    bgfx_frame(false);

    ImGuiTestEngine_PostSwap(engine);
  }

  destroy_pos_color_lines(pos_color_lines);
  destroy_pos_color_quads(pos_color_quads);

  bgfx_destroy_uniform(u_color);
  bgfx_destroy_program(program);

  mc_gol_destroy_board(board);

  ImGuiTestEngine_Stop(engine);
  ImGui_ImplSDL2_Shutdown();
  ImGui_Implbgfx_Shutdown();
  ImGui::DestroyContext();
  ImGuiTestEngine_DestroyContext(engine);
  bgfx_shutdown();
  SDL_DestroyWindow(window);
  SDL_Quit();

  return exit_code;
}
