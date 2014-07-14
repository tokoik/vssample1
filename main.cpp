#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>

// 補助プログラム
#include "gg.h"
using namespace gg;

// アニメーションの周期（秒）
const double cycle(5.0);

//
// ウィンドウ関連の処理
//
class Window
{
  // ウィンドウの識別子
  GLFWwindow *const window;
  
  // 透視投影変換行列
  GgMatrix mp;
  
  // トラックボール
  GgTrackball tb;
  
public:
  
  // コンストラクタ
  Window(const char *title = "Game Graphics", int width = 640, int height = 480)
  : window(glfwCreateWindow(width, height, title, NULL, NULL))
  {
    if (window == NULL)
    {
      // ウィンドウが作成できなかった
      std::cerr << "Can't create GLFW window." << std::endl;
      exit(1);
    }
    
    // 現在のウィンドウを処理対象にする
    glfwMakeContextCurrent(window);
    
    // 作成したウィンドウに対する設定
    glfwSwapInterval(1);
    
    // ゲームグラフィックス特論の都合にもとづく初期化
    ggInit();
    
    // このインスタンスの this ポインタを記録しておく
    glfwSetWindowUserPointer(window, this);
    
    // マウスボタンを操作したときの処理
    glfwSetMouseButtonCallback(window, mouse);
    
    // ウィンドウのサイズ変更時に呼び出す処理を登録する
    glfwSetFramebufferSizeCallback(window, resize);
    
    // ウィンドウの設定を初期化する
    resize(window, width, height);
  }
  
  // デストラクタ
  virtual ~Window()
  {
    glfwDestroyWindow(window);
  }
  
  // ウィンドウを閉じるべきかを判定する
  int shouldClose() const
  {
    return glfwWindowShouldClose(window) | glfwGetKey(window, GLFW_KEY_ESCAPE);
  }
  
  // カラーバッファを入れ替えてイベントを取り出す
  void swapBuffers()
  {
    // カラーバッファを入れ替える
    glfwSwapBuffers(window);
    
    // OpenGL のエラーをチェックする
    ggError("SwapBuffers");
    
    // イベントを取り出す
    glfwPollEvents();
    
    // 左ボタンドラッグ
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
    {
      // マウスの現在位置を取得する
      double x, y;
      glfwGetCursorPos(window, &x, &y);
      
      // トラックボール処理
      tb.motion(static_cast<float>(x), static_cast<float>(y));
    }
  }
  
  // マウスボタンを操作したときの処理
  static void mouse(GLFWwindow *window, int button, int action, int mods)
  {
    // このインスタンスの this ポインタを得る
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));
    
    if (instance)
    {
      // マウスカーソルの現在位置を取得する
      double x, y;
      glfwGetCursorPos(window, &x, &y);
      
      switch (button)
      {
        case GLFW_MOUSE_BUTTON_1:
          // トラックボール処理
          if (action)
          {
            // トラックボール処理開始
            instance->tb.start(static_cast<float>(x), static_cast<float>(y));
          }
          else
          {
            // トラックボール処理終了
            instance->tb.stop(static_cast<float>(x), static_cast<float>(y));
          }
          break;
        case GLFW_MOUSE_BUTTON_2:
          break;
        case GLFW_MOUSE_BUTTON_3:
          break;
        default:
          break;
      }
    }
  }
  
  // ウィンドウのサイズ変更時の処理
  static void resize(GLFWwindow *window, int width, int height)
  {
    // ウィンドウ全体をビューポートにする
    glViewport(0, 0, width, height);
    
    // このインスタンスの this ポインタを得る
    Window *const instance(static_cast<Window *>(glfwGetWindowUserPointer(window)));
    
    if (instance != NULL)
    {
      // 透視投影変換行列を求める（アスペクト比 w / h）
      instance->mp.loadPerspective(0.8f, (float)width / (float)height, 2.0f, 4.5f);
      
      // トラックボール処理の範囲を設定する
      instance->tb.region(width, height);
    }
  }
  
  // 投影変換行列を取り出す
  const GgMatrix &getMp() const
  {
    return mp;
  }
  
  // トラックボールの変換行列を取り出す
  const GLfloat *getTb() const
  {
    return tb.get();
  }
};

//
// プログラム終了時の処理
//
static void cleanup()
{
  // GLFW の終了処理
  glfwTerminate();
}

//
// メインプログラム
//
int main(int argc, const char * argv[])
{
  // GLFW を初期化する
  if (glfwInit() == GL_FALSE)
  {
    // 初期化に失敗した
    std::cerr << "Can't initialize GLFW" << std::endl;
    return 1;
  }
  
  // プログラム終了時の処理を登録する
  atexit(cleanup);
  
  // OpenGL Version 3.2 Core Profile を選択する
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  // ウィンドウを作成する
  Window window("Vertex Shader Sample 1");
  
  // 背景色を指定する
  glClearColor(0.2f, 0.3f, 0.6f, 0.0f);
  
  // 隠面消去を有効にする
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

  // プログラムオブジェクトの作成
  const GLuint program(ggLoadShader("simple.vert", "simple.frag"));
  
  // uniform 変数のインデックスの検索（見つからなければ -1）
  const GLint mcLoc(glGetUniformLocation(program, "mc"));
  const GLint tLoc(glGetUniformLocation(program, "t"));
  
  // ビュー変換行列を mv に求める
  const GgMatrix mv(ggLookat(2.0f, 1.5f, 2.5f, 0.0f, -0.2f, 0.0f, 0.0f, 1.0f, 0.0f));
  
  // 図形データの作成
  const std::unique_ptr<const GgPoints> cube(ggPointsCube(10000, 2.0f));
  
  // 経過時間のリセット
  glfwSetTime(0.0);
  
  // ウィンドウが開いている間くり返し描画する
  while (window.shouldClose() == GL_FALSE)
  {
    // ウィンドウを消去する
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // シェーダプログラムの使用開始
    glUseProgram(program);
    
    // モデルビュー変換
    const GgMatrix mw(mv * window.getTb());

    // モデルビュー・投影変換
    const GgMatrix mc(window.getMp() * mw);
    
    // uniform 変数を設定する
    glUniformMatrix4fv(mcLoc, 1, GL_FALSE, mc.get());
    
    // 時刻の計測
    glUniform1f(tLoc, static_cast<float>(fmod(glfwGetTime(), cycle) / cycle));
    
    // 図形の描画
    cube->draw();
    
    // シェーダプログラムの使用終了
    glUseProgram(0);
    
    // カラーバッファを入れ替えてイベントを取り出す
    window.swapBuffers();
  }
  
  return 0;
}
