#include "GUI.hpp"
#include "Camera.hpp"
#include "PBuffer.hpp"
#include "Particle.hpp"
#include "Solver.hpp"
#include "common.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <fstream>

#include <utility>

GUI::GUI(int WIDTH, int HEIGHT) : width(WIDTH), height(HEIGHT)
{
  // OpenGL initialization
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  // assign window
  window = glfwCreateWindow(WIDTH, HEIGHT, "SPH (fps: 60)", nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
  }

  glViewport(0, 0, WIDTH, HEIGHT);
}

void
GUI::mainLoop(const std::function<void()> &callback)
{
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT);
    callback();
    glfwSwapBuffers(window);
  }
}

RTGUI::RTGUI(int WIDTH, int HEIGHT)
    : GUI::GUI(WIDTH, HEIGHT), lastX(WIDTH / 2.0), lastY(HEIGHT / 2.0)
{
  p_shader = std::make_unique<Shader>();
  // clang-format off
  m_shader =
      std::make_unique<Shader>("src/shaders/m_vert.glsl"s, "src/shaders/m_frag.glsl"s);
  // clang-format on
  camera = std::make_unique<OrbitCamera>();

  // Setup Dear ImGui context
  const char *glsl_version = "#version 330 core";
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
  ImGui::StyleColorsClassic();

  std::cout << "gui initialization finished " << std::endl;
}

void
RTGUI::setSolver(std::shared_ptr<Solver> _solver)
{
  solver = std::move(_solver);
}

void
RTGUI::setParticles(const PBuffer &_buffer)
{
  buffer = _buffer;
  if (VAO != 0) glDeleteVertexArrays(1, &VAO);
  if (VBO != 0) glDeleteBuffers(1, &VBO);
  if (EBO != 0) glDeleteBuffers(1, &EBO);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  std::vector<float> points;
  points.reserve(buffer->size() * 4);
  for (auto &i : (*buffer)) {
    points.push_back(i.pos.x);
    points.push_back(i.pos.y);
    points.push_back(i.pos.z);
    points.push_back(i.rho);
  }

  glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(),
               GL_STREAM_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 4,
                        (void *)nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 4,
                        (void *)(3 * sizeof(float)));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
RTGUI::mainLoop(const std::function<void()> &callback)
{
  // Call setParticles in subStep function and return the newly
  // generated particles

  std::cout << "entered mainLoop" << std::endl;
  while (!glfwWindowShouldClose(window)) {
    ++frame;

    // Do something with particles
    glfwPollEvents();
    glClearColor(0.16, 0.17, 0.2, 1); // one dark
    // glClearColor(0.921f, 0.925f, 0.933f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    processInput();
    refreshFps();

    if (!pause) { callback(); }
    renderParticles();
    if (snapshot) {
      saveParticles();
      snapshot = false;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImGui widgets
    {
      // ImGui::SetNextWindowSize(ImVec2(360, 150), ImGuiCond_Always);
      ImGuiIO &io = ImGui::GetIO();
      ImGui::Begin("SPH Controller", nullptr,
                   ImGuiWindowFlags_AlwaysAutoResize);
      ImGui::Text("Framerate: %.1f", io.Framerate);
      ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glfwSwapBuffers(window);
  }
}

void
RTGUI::renderParticles() const
{
  static float rotate_y = 45.0f;

  auto model = glm::translate(glm::mat4(1.0f), vec3(0));
  model = glm::rotate(model, rotate_y, vec3(0.0f, 1.0f, 0.0f));
  model = glm::scale(model, vec3(1 / solver->getBorder()));

  // auto camera_pos = vec3(0.0f, 3.0f, 6.0f);
  // auto camera_center = vec3(0.0f);
  // auto camera_up = vec3(0.0f, 1.0f, 0.0f);
  // auto view = glm::lookAt(camera_pos, camera_center, camera_up);

  auto view = camera->getViewMatrix();
  auto projection = glm::perspective(
      glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 100.0f);

  glBindVertexArray(VAO);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  p_shader->use();
  p_shader->set_mat4("model", model);
  p_shader->set_mat4("view", view);
  p_shader->set_mat4("projection", projection);
  glPointSize(6.2f);
  glDrawArrays(GL_POINTS, 0, buffer->size());

  assert(m_shader != nullptr);
  glBindVertexArray(0);
}

void
RTGUI::del()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  p_shader->del();
  m_shader->del();

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glfwDestroyWindow(window);
  glfwTerminate();
}

void
RTGUI::processInput()
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS) {
    std::clog << "sph quited" << std::endl;
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
    std::clog << "saving particles" << std::endl;
    snapshot = true;
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    if (lastPauseFrame != 0 && frame - lastPauseFrame < 20) {
      // std::clog << "pause ignored" << std::endl;
    } else {
      std::clog << "pause switched to: " << !pause << std::endl;
      pause = !pause;
      lastPauseFrame = frame;
    }
  }

  // TODO: USe mouse_callback to improve performance
  GLdouble xPos, yPos;
  glfwGetCursorPos(window, &xPos, &yPos);
  mouseCallback(xPos, yPos);
}

void
RTGUI::refreshFps() const
{
  static int n_frames = 0;
  static auto last_time = glfwGetTime();
  auto cur_time = glfwGetTime();
  auto delta = cur_time - last_time;
  ++n_frames;

  if (delta > 1.0f) {
    std::stringstream win_title;
    win_title << "sph (fps: " << n_frames / delta << ")";
    glfwSetWindowTitle(window, win_title.str().c_str());
    n_frames = 0;
    last_time = cur_time;
  }
}

void
RTGUI::saveParticles() const
{
  std::ofstream of;

  std::string filename = "particles_" + std::to_string(frame) + ".txt";
  of.open(filename);

  float scale = 3.0f;
  of << buffer->size() << std::endl;
  for (uint i = 0; i < buffer->size(); ++i) {
    of << "p " << (*buffer)[i].pos.x / scale << " " << (*buffer)[i].pos.y / scale << " "
       << (*buffer)[i].pos.z / scale << std::endl;
  }

  of.close();
  std::cout << "particles saved to: " << filename << std::endl;
}

void
RTGUI::mouseCallback(double x, double y)
{
  x = (float)x;
  y = (float)y;

  if (firstMouse) {
    lastX = x;
    lastY = y;
    firstMouse = false;
  }

  float xOffset = x - lastX;
  float yOffset = lastY - y;

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
    camera->processDrag(xOffset, yOffset);
    // std::cout << xOffset << " " << yOffset << std::endl;
  }

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
    camera->processScroll(yOffset);
  }

  lastX = x;
  lastY = y;
}
