#include "GLTools.h"
#include "GLMatrixStack.h"
#include "GLFrame.h"
#include "GLFrustum.h"
#include "GLGeometryTransform.h"
#include "glew.h"

#include <math.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/glut.h>
#endif
#pragma mark - 变量部分
/*  public  */
GLfloat  vRed[] = {1.f,0.f,0.f,1.f};
GLfloat  vGreen[] = {0.f,1.f,0.f,1.f};
GLfloat  vBlue[] = {0.f,0.f,1.f,1.f};
GLfloat  vBlace[] = {0.f,0.f,0.f,1.f};

GLfloat blockSize = 0.1f;

int iCull = 0;
int iDepth = 0;

//1 三角形  2 正方形  3 点、线、三角形、伞、金字塔、手环  4 甜甜圈
int nDrawCategory = 1;
int nSteps = 0;

//是否移动物体
bool nMoveObject = false;

GLShaderManager  shaderManger;
GLFrame   cameraFrame;
GLFrame   objectFrame;
GLFrustum  viewFrustum;
GLGeometryTransform  transformPipleLine;

GLMatrixStack  modelViewStack;
GLMatrixStack  projectionMatrixStack;

/** 正方形、三角形键位控制 **/
GLBatch  triangleBatch;
GLBatch  squareBatch;

GLfloat  xPos = 0;
GLfloat  yPos = 0;


/**  点、线、三角形、伞、金字塔、手环    **/
GLBatch pointBatch;
GLBatch lineBatch;
GLBatch lineStripBatch;
GLBatch lineLoopBatch;
GLBatch pyramidBatch;
GLBatch umbrellaBatch;
GLBatch ringBatch;
GLTriangleBatch torusBatch;


/**  甜甜圈    **/

#pragma mark - 顶点数据  -   三角形demo
void vertexDataTriangle() {
    GLfloat vTri[] = {
        -blockSize,0.f,0.f,
        blockSize,0.f,0.f,
        0.f,blockSize,0.f
    };
    
    triangleBatch.Begin(GL_TRIANGLES, 3);
    triangleBatch.CopyVertexData3f(vTri);
    triangleBatch.End();
}

#pragma mark - 顶点数据  -   正方形demo
void vertexDataSquare() {
    GLfloat vSqu[] = {
        -blockSize,-blockSize,0.f,
        blockSize,-blockSize,0.f,
        blockSize,blockSize,0.f,
        -blockSize,blockSize,0.f
    };
    
    squareBatch.Begin(GL_TRIANGLE_FAN, 4);
    squareBatch.CopyVertexData3f(vSqu);
    squareBatch.End();
}

#pragma mark - 顶点数据  -   基本图形demo
//point
void vertexDataPoint() {
    GLfloat vPoints[] = {
        3,3,0,
        0,3,0,
        3,0,0
    };
    
    pointBatch.Begin(GL_POINTS, 3);
    pointBatch.CopyVertexData3f(vPoints);
    pointBatch.End();
}

//线
void vertexDataLine() {
    GLfloat vPoints[] = {
        3,3,0,
        0,3,0,
        3,0,0
    };
    
    lineBatch.Begin(GL_LINES, 3);
    lineBatch.CopyVertexData3f(vPoints);
    lineBatch.End();
}

//线段
void vertexDataLineStrip() {
    GLfloat vPoints[] = {
        3,3,0,
        0,3,0,
        3,0,0
    };
    
    lineStripBatch.Begin(GL_LINE_STRIP, 3);
    lineStripBatch.CopyVertexData3f(vPoints);
    lineStripBatch.End();
}

//线段头尾相接
void vertexDataLineStripLoop() {
    GLfloat vPoints[] = {
        3,3,0,
        0,3,0,
        3,0,0
    };
    
    lineLoopBatch.Begin(GL_LINE_LOOP, 3);
    lineLoopBatch.CopyVertexData3f(vPoints);
    lineLoopBatch.End();
}

//金字塔
void vertexDataPyramid() {
    GLfloat vPyramid[12][3] = {
        -2.f,0.f,-2.f,
        2.f,0.f,-2.f,
        0,4.f,0,
        
        2.f,0.f,-2.f,
        2.f,0.f,2.f,
        0,4.f,0,
        
        2.f,0,2.f,
        -2.f,0,2.f,
        0,4.f,0,
        
        -2.f,0,2.f,
        -2.f,0,-2.f,
        0,4.f,0
    };
    
    pyramidBatch.Begin(GL_TRIANGLES, 12);
    pyramidBatch.CopyVertexData3f(vPyramid);
    pyramidBatch.End();
}


//伞
void vertexDataUmbrella() {
    GLfloat vUmb[100][3];
    
    int nVerts = 0;
    vUmb[nVerts][0] = 0.f;
    vUmb[nVerts][1] = 0.f;
    vUmb[nVerts][2] = 0.f;
    
    GLfloat r = 3.f;
    for (GLfloat angle = 0; angle < M3D_2PI; angle += M3D_2PI/6) {
        nVerts += 1;
        
        GLfloat sinValue = float(sin(angle));
        GLfloat cosValue = float(cos(angle));
        
        
        vUmb[nVerts][0] = cosValue* r;
        vUmb[nVerts][1] = sinValue * r;
        vUmb[nVerts][2] = -0.5f;
    }
    
    nVerts += 1;
    
    vUmb[nVerts][0] = r;
    vUmb[nVerts][1] = 0;
    vUmb[nVerts][2] = 0;
    
    umbrellaBatch.Begin(GL_TRIANGLE_FAN, 8);
    umbrellaBatch.CopyVertexData3f(vUmb);
    umbrellaBatch.End();
}

//环
void vertexDataRing() {
    GLfloat vRing[100][3];
    
    int iCounter = 0;
    GLfloat r = 3.f;
    
    for (GLfloat angle = 0; angle < M3D_2PI; angle += 0.3f) {
        GLfloat sinValue = float(sin(angle));
        GLfloat cosValue = float(cos(angle));
        
        //绘制两个三角形，两个三角形x、y顶点相同，z不同，
        //只需要确定x、y一样z不同的两个顶点，由于后续使用GL_TRIANGLE_STRIP绘制
        //正值的z值的点与下一个负值的z值的点之间也会连线
        GLfloat x = cosValue * r;
        GLfloat y = sinValue * r;
        vRing[iCounter][0] = x;
        vRing[iCounter][1] = y;
        vRing[iCounter][2] = -0.5f;
        
        iCounter += 1;
        
        vRing[iCounter][0] = x;
        vRing[iCounter][1] = y;
        vRing[iCounter][2] = 0.5f;
        
        iCounter += 1;
    }
    
    //关闭循环
    vRing[iCounter][0] = vRing[0][0];
    vRing[iCounter][1] = vRing[0][1];
    vRing[iCounter][2] = -0.5f;
    
    iCounter += 1;
    
    vRing[iCounter][0] = vRing[0][0];
    vRing[iCounter][1] = vRing[0][1];
    vRing[iCounter][2] = 0.5f;
    iCounter += 1;
    
    ringBatch.Begin(GL_TRIANGLE_STRIP, iCounter);
    ringBatch.CopyVertexData3f(vRing);
    ringBatch.End();
}

#pragma mark - 甜甜圈
//画甜甜圈
void vertexDataTorus() {
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);
}

#pragma mark - RenderScence Private
/*
1、  平面着色器根据顶点绘画图形
2、  经过多边形偏移，因为在同一位置绘制黑色填充和边线，会产生z值冲突
3、  抗锯齿
4、  开启并设置颜色混合
5、  设置填充模式为正背面 画线模式
6、  画边框
7、  关闭抗锯齿、多边形偏移、填充模式、颜色混合等初始化操作
*/
void DrawWireFramedBatch(GLBatch* pBatch)
{
    /*------------画绿色部分----------------*/
    /* GLShaderManager 中的Uniform 值——平面着色器
     参数1：平面着色器
     参数2：运行为几何图形变换指定一个 4 * 4变换矩阵
          --transformPipeline 变换管线（指定了2个矩阵堆栈）
     参数3：颜色值
    */
    shaderManger.UseStockShader(GLT_SHADER_FLAT, transformPipleLine.GetModelViewProjectionMatrix(), vGreen);
    pBatch->Draw();
    
    /*-----------边框部分-------------------*/
    /*
        glEnable(GLenum mode); 用于启用各种功能。功能由参数决定
        参数列表：http://blog.csdn.net/augusdi/article/details/23747081
        注意：glEnable() 不能写在glBegin() 和 glEnd()中间
        GL_POLYGON_OFFSET_LINE  根据函数glPolygonOffset的设置，启用线的深度偏移
        GL_LINE_SMOOTH          执行后，过虑线点的锯齿
        GL_BLEND                启用颜色混合。例如实现半透明效果
        GL_DEPTH_TEST           启用深度测试 根据坐标的远近自动隐藏被遮住的图形（材料
     
     
        glDisable(GLenum mode); 用于关闭指定的功能 功能由参数决定
     
     */
    
    //画黑色边框
    glPolygonOffset(-1.0f, -1.0f);// 偏移深度，在同一位置要绘制填充和边线，会产生z冲突，所以要偏移
    glEnable(GL_POLYGON_OFFSET_LINE);
    
    // 画反锯齿，让黑边好看些
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //绘制线框几何黑色版 三种模式，实心，边框，点，可以作用在正面，背面，或者两面
    //通过调用glPolygonMode将多边形正面或者背面设为线框模式，实现线框渲染
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //设置线条宽度
    glLineWidth(2.5f);
    
    /* GLShaderManager 中的Uniform 值——平面着色器
     参数1：平面着色器
     参数2：运行为几何图形变换指定一个 4 * 4变换矩阵
         --transformPipeline.GetModelViewProjectionMatrix() 获取的
          GetMatrix函数就可以获得矩阵堆栈顶部的值
     参数3：颜色值（黑色）
     */
    
    shaderManger.UseStockShader(GLT_SHADER_FLAT, transformPipleLine.GetModelViewProjectionMatrix(), vBlace);
    pBatch->Draw();

    // 复原原本的设置
    //通过调用glPolygonMode将多边形正面或者背面设为全部填充模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
    
    
}

//压入栈操作，模型视图矩阵*观察者矩阵*投影矩阵
void publicPushStackOperation() {
    //复制一份单元矩阵
    modelViewStack.PushMatrix();
    M3DMatrix44f cameraMatrix;
    
    cameraFrame.GetCameraMatrix(cameraMatrix);
    modelViewStack.MultMatrix(cameraMatrix);
    
    M3DMatrix44f objectMatrix;
    objectFrame.GetMatrix(objectMatrix);
    
    modelViewStack.MultMatrix(objectMatrix);
    
    shaderManger.UseStockShader(GLT_SHADER_FLAT,transformPipleLine.GetModelViewProjectionMatrix(),vBlue);
}

//出栈
void publicPopOperation() {
    modelViewStack.PopMatrix();
}

//渲染点
void renderPoint() {
    publicPushStackOperation();
    
    glPointSize(10.f);
    pointBatch.Draw();
    glPointSize(1.f);
    
    publicPopOperation();
}

//渲染线
void renderLine() {
    publicPushStackOperation();
    
    glLineWidth(5.f);
    lineBatch.Draw();
    glLineWidth(1.f);
    
    publicPopOperation();
}

//渲染线段
void renderLineStrip() {
    publicPushStackOperation();
    
    glLineWidth(5.f);
    lineStripBatch.Draw();
    glLineWidth(1.f);
    
    publicPopOperation();
}

//渲染线段三角形
void renderLineStripLoop() {
    publicPushStackOperation();
    
    glLineWidth(5.f);
    lineLoopBatch.Draw();
    glLineWidth(1.f);
    
    publicPopOperation();
}

//渲染金字塔
void renderPyramid() {
    publicPushStackOperation();
    
    
    DrawWireFramedBatch(&pyramidBatch);
    publicPopOperation();
}

//渲染伞形
void renderUmbrella() {
    publicPushStackOperation();
    
    DrawWireFramedBatch(&umbrellaBatch);
    publicPopOperation();
}

//渲染环
void renderRing() {
    publicPushStackOperation();
    
    DrawWireFramedBatch(&ringBatch);
    publicPopOperation();
}

//渲染几种基本图形
void renderCategoryNormalSquare() {
    switch (nSteps) {
        case 0:
            renderPoint();
            glutSetWindowTitle("Point");
            break;
        case 1:
            renderLine();
            glutSetWindowTitle("Line");
            break;
        case 2:
            renderLineStrip();
            glutSetWindowTitle("Line Segment");
            break;
        case 3:
            renderLineStripLoop();
            glutSetWindowTitle("Line Segment Triangle");
            break;
        case 4:
            renderPyramid();
            glutSetWindowTitle("Pyramid");
            break;
        case 5:
            renderUmbrella();
            glutSetWindowTitle("Umbrella");
            break;
        case 6:
            renderRing();
            glutSetWindowTitle("Ring");
            break;
        default:
            break;
    }
}

//渲染甜甜圈
void renderTorus() {
    if (iCull) {
        glEnable(GL_CULL_FACE);
        //设置顶点顺序逆时针为正面
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }
    
    
    if (iDepth) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    modelViewStack.PushMatrix(cameraFrame);
    shaderManger.UseStockShader(GLT_SHADER_DEFAULT_LIGHT,transformPipleLine.GetModelViewMatrix(),transformPipleLine.GetProjectionMatrix(),vBlue);
    glPointSize(4.0f);
    torusBatch.Draw();
    glPointSize(1.f);
    
    glutSetWindowTitle("Torus");
    publicPopOperation();
}

#pragma mark - Main Methods
void ProcessMenu(int value) {
    switch (value) {
        case 1:
            iDepth = !iDepth;
            break;
        case 2:
            iCull = !iCull;
            break;
        case 3:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case 4:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case 5:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
            
        default:
            break;
    }
    
    glutPostRedisplay();
}

void ChangeSize(int w, int h) {
    glViewport(0, 0, w, h);
    
    //创建投影矩阵
    viewFrustum.SetPerspective(35.f, float(w)/float(h), 1.f, 500.f);
    //投影矩阵载入投影矩阵堆栈
    projectionMatrixStack.LoadMatrix(viewFrustum.GetProjectionMatrix());
    
    modelViewStack.LoadIdentity();
}

void RenderScence() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //变换矩阵
    M3DMatrix44f mTransformMatrix;
    
    switch (nDrawCategory) {
        case 1:
            m3dTranslationMatrix44(mTransformMatrix, xPos, yPos, 0);
            shaderManger.UseStockShader(GLT_SHADER_FLAT,mTransformMatrix,vRed);
            triangleBatch.Draw();
            glutSetWindowTitle("Triangle");
            break;
        case 2:
            m3dTranslationMatrix44(mTransformMatrix, xPos, yPos, 0);
            shaderManger.UseStockShader(GLT_SHADER_FLAT,mTransformMatrix,vRed);
            glutSetWindowTitle("Square");
            squareBatch.Draw();
            break;
        case 3:
            renderCategoryNormalSquare();
            break;
        case 4:
            renderTorus();
            break;
        default:
            break;
    }
    
    glutSwapBuffers();
}

void SpecialKeys(int key,int x,int y) {
    if (key == GLUT_KEY_F1) {
        //F1键控制demo类型
        nDrawCategory += 1;
        if (nDrawCategory > 4) {
            nDrawCategory = 1;
        }
        xPos = 0;
        yPos = 0;
        iCull = 0;
        iDepth = 0;
        nSteps = 0;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        if (nDrawCategory == 4) {
            //改变视角距离的原因是  绘画甜甜圈的时候，观察者动，物体不懂
            cameraFrame.MoveForward(22.f);
            nMoveObject = false;
        } else {
            nMoveObject = true;
        }
        
        if (nDrawCategory == 1) {
            //改变视角距离的原因是  绘画甜甜圈的时候，观察者动，物体不懂
            cameraFrame.MoveForward(-22.f);
        }
    }
    if (key == 32 &&
        nDrawCategory == 3) {
        //空格键控制绘制基本图形的时候，图形类型切换
        nSteps += 1;
        
        if (nSteps > 6) {
            nSteps = 0;
        }
    }
    
    //用作平面图形的移动
    GLfloat stepSize = 0.025;
    bool  is3D = (nDrawCategory > 2);
    //上下左右 分别旋转
    if (key == GLUT_KEY_LEFT) {
        if (is3D) {
            //三角形、正方形demo不旋转，其他旋转
            if (nMoveObject) {
                objectFrame.RotateWorld(m3dDegToRad(-5.0), 0, 1, 0);
            } else {
                cameraFrame.RotateWorld(m3dDegToRad(-5.0), 0, 1, 0);
            }
        } else {
            xPos -= stepSize;
        }
        
    } else if (key == GLUT_KEY_RIGHT) {
        if (is3D) {
            //三角形、正方形demo不旋转，其他旋转
            if (nMoveObject) {
                objectFrame.RotateWorld(m3dDegToRad(5.0), 0, 1, 0);
            } else {
                cameraFrame.RotateWorld(m3dDegToRad(5.0), 0, 1, 0);
            }
        } else {
            xPos += stepSize;
        }
    } else if (key == GLUT_KEY_UP) {
        if (is3D) {
            //三角形、正方形demo不旋转，其他旋转
            if (nMoveObject) {
                objectFrame.RotateWorld(m3dDegToRad(5.0), 1, 0, 0);
            } else {
                cameraFrame.RotateWorld(m3dDegToRad(5.0), 1, 0, 0);
            }
        } else {
            yPos += stepSize;
        }
    } else if (key == GLUT_KEY_DOWN) {
        if (is3D) {
            //三角形、正方形demo不旋转，其他旋转
            if (nMoveObject) {
                objectFrame.RotateWorld(m3dDegToRad(-5.0), 1, 0, 0);
            } else {
                cameraFrame.RotateWorld(m3dDegToRad(-5.0), 1, 0, 0);
            }
        } else {
            yPos -= stepSize;
        }
    }
    
    if (!is3D) {
        //需要做图形是否超出视口的检测
        if (xPos < -1.f + blockSize) {
            xPos = -1.f + blockSize;
        } else if (xPos > 1.f - blockSize) {
            xPos = 1.f - blockSize;
        }
        
        if (yPos < -1.f + blockSize) {
            yPos = -1.f + blockSize;
        } else if (yPos > 1.f - blockSize) {
            yPos = 1.f - blockSize;
        }
    }
    
    glutPostRedisplay();
}

void SetupRC() {
    glClearColor(1,1,1,1.f);
    
    shaderManger.InitializeStockShaders();
    
    transformPipleLine.SetMatrixStacks(modelViewStack, projectionMatrixStack);
    //设置观察者动
    nMoveObject = false;
    cameraFrame.MoveForward(-15.f);
    
    //三角形顶点数据
    vertexDataTriangle();
    
    //正方形顶点数据
    vertexDataSquare();
    
    
    vertexDataPoint();
    
    vertexDataLine();
    vertexDataLineStrip();
    vertexDataLineStripLoop();
    vertexDataPyramid();
    vertexDataUmbrella();
    vertexDataRing();
    vertexDataTorus();
}

int main(int argc, char * argv[]) {
    //mac  设置工作空间
    gltSetWorkingDirectory(argv[0]);
    //初始化glut窗口
    glutInit(&argc, argv);
    //初始化渲染模式
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    //初始化窗口大小
    glutInitWindowSize(500, 500);
    //初始化窗口，设置title
    glutCreateWindow("draw");
    
    //设置重塑函数
    glutReshapeFunc(ChangeSize);
    //设置渲染回调函数
    glutDisplayFunc(RenderScence);
    //设置特殊键位回调函数
    glutSpecialFunc(SpecialKeys);
    
    //创建一个菜单
    glutCreateMenu(ProcessMenu);
    glutAddMenuEntry("Toggle depth test", 1);
    glutAddMenuEntry("Toggle cull backface", 2);
    glutAddMenuEntry("Set Fill Mode", 3);
    glutAddMenuEntry("Set Line Mode", 4);
    glutAddMenuEntry("Set Point Mode", 5);
    
    //右键开启菜单
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }
    
    SetupRC();
    glutMainLoop();
    
    return 0;
}


