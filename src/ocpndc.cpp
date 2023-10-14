#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "config.h"

#include "dychart.h"
#include "ocpn_plugin.h"
#include "chcanv.h"
#include "linmath.h"
#include "ocpn_frame.h"

#ifdef __MSVC__
#include <windows.h>
#endif

#ifdef ocpnUSE_GL
#include "shaders.h"
#endif

#include <wx/graphics.h>
#include <wx/dcclient.h>

#include <vector>

#include "ocpndc.h"
#include "wx28compat.h"
#include "cutil.h"

wxArrayPtrVoid gTesselatorVertices;

#if defined(USE_ANDROID_GLES2) || defined(ocpnUSE_GLSL)
extern GLint color_tri_shader_program;
extern GLint circle_filled_shader_program;
extern GLint texture_2D_shader_program;
#endif

//----------------------------------------------------------------------------
//   constants
//----------------------------------------------------------------------------
#ifndef PI
#define PI 3.1415926535897931160E0 /* pi */
#endif

//----------------------------------------------------------------------------
ocpnDC::ocpnDC(wxGLCanvas &canvas)
    : m_glcanvas(&canvas), dc(NULL), m_pen(wxNullPen), m_brush(wxNullBrush) {
#if wxUSE_GRAPHICS_CONTEXT
  pgc = NULL;
#endif
  Init();
}

ocpnDC::ocpnDC(wxDC &pdc)
    : m_glcanvas(NULL), dc(&pdc), m_pen(wxNullPen), m_brush(wxNullBrush) {
#if wxUSE_GRAPHICS_CONTEXT
  pgc = NULL;
  wxMemoryDC *pmdc = wxDynamicCast(dc, wxMemoryDC);
  if (pmdc)
    pgc = wxGraphicsContext::Create(*pmdc);
  else {
    wxClientDC *pcdc = wxDynamicCast(dc, wxClientDC);
    if (pcdc) pgc = wxGraphicsContext::Create(*pcdc);
  }
#endif

  Init();
}

ocpnDC::ocpnDC()
    : m_glcanvas(NULL), dc(NULL), m_pen(wxNullPen), m_brush(wxNullBrush) {
#if wxUSE_GRAPHICS_CONTEXT
  pgc = NULL;
#endif
  Init();
}

ocpnDC::~ocpnDC() {
#if wxUSE_GRAPHICS_CONTEXT
  if (pgc) delete pgc;
#endif
  free(workBuf);

  free(s_odc_tess_work_buf);

  delete m_pcolor_tri_shader_program;
  delete m_pAALine_shader_program;
  delete m_pcircle_filled_shader_program;
  delete m_ptexture_2D_shader_program;
}

void ocpnDC::Init(){
  m_buseTex = GetLocaleCanonicalName().IsSameAs(_T("en_US"));
  workBuf = NULL;
  workBufSize = 0;
  s_odc_tess_work_buf = NULL;
  m_dpi_factor = 1.0;
  m_canvasIndex = 0;
  m_textforegroundcolour = wxColour(0, 0, 0);

#if defined(USE_ANDROID_GLES2) || defined(ocpnUSE_GLSL)
  s_odc_tess_vertex_idx = 0;
  s_odc_tess_vertex_idx_this = 0;
  s_odc_tess_buf_len = 0;

  s_odc_tess_work_buf = (GLfloat *)malloc(100 * sizeof(GLfloat));
  s_odc_tess_buf_len = 100;

  m_pcolor_tri_shader_program = NULL;
  m_pAALine_shader_program = NULL;
  m_pcircle_filled_shader_program = NULL;
  m_ptexture_2D_shader_program = NULL;
#endif

}

void ocpnDC::Clear() {
  if (dc)
    dc->Clear();
  else {
#ifdef ocpnUSE_GL
    wxBrush tmpBrush = m_brush;
    int w, h;    
    
    DrawRectangle(0, 0, w, h);
    SetBrush(tmpBrush);
#endif
  }
}

void ocpnDC::SetBackground(const wxBrush &brush) {
  if (dc) dc->SetBackground(brush);  
}

void ocpnDC::SetPen(const wxPen &pen) {
  if (dc) {
    if (pen == wxNullPen)
      dc->SetPen(*wxTRANSPARENT_PEN);
    else
      dc->SetPen(pen);
  } else
    m_pen = pen;
}

void ocpnDC::SetBrush(const wxBrush &brush) {
  if (dc)
    dc->SetBrush(brush);
  else
    m_brush = brush;
}

void ocpnDC::SetTextForeground(const wxColour &colour) {
  if (dc)
    dc->SetTextForeground(colour);
  else
    m_textforegroundcolour = colour;
}

void ocpnDC::SetFont(const wxFont &font) {
  if (dc)
    dc->SetFont(font);
  else
    m_font = font;
}

const wxPen &ocpnDC::GetPen() const {
  if (dc) return dc->GetPen();
  return m_pen;
}

const wxBrush &ocpnDC::GetBrush() const {
  if (dc) return dc->GetBrush();
  return m_brush;
}

const wxFont &ocpnDC::GetFont() const {
  if (dc) return dc->GetFont();
  return m_font;
}

void ocpnDC::GetSize(wxCoord *width, wxCoord *height) const {
  if (dc) dc->GetSize(width, height);  
}

void ocpnDC::DrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2,
                      bool b_hiqual) {
  if (dc) dc->DrawLine(x1, y1, x2, y2);
}

void ocpnDC::DrawLines(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, bool b_hiqual) {
  if (dc) dc->DrawLines(n, points, xoffset, yoffset);
}

void ocpnDC::StrokeLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) {
#if wxUSE_GRAPHICS_CONTEXT
  if (pgc) {
    pgc->SetPen(dc->GetPen());
    pgc->StrokeLine(x1, y1, x2, y2);

    dc->CalcBoundingBox(x1, y1);
    dc->CalcBoundingBox(x2, y2);
  } else
#endif
    DrawLine(x1, y1, x2, y2, true);
}

void ocpnDC::StrokeLines(int n, wxPoint *points) {
  if (n < 2) /* optimization and also to avoid assertion in pgc->StrokeLines */
    return;

#if wxUSE_GRAPHICS_CONTEXT
  if (pgc) {
    wxPoint2DDouble *dPoints =
        (wxPoint2DDouble *)malloc(n * sizeof(wxPoint2DDouble));
    for (int i = 0; i < n; i++) {
      dPoints[i].m_x = points[i].x;
      dPoints[i].m_y = points[i].y;
    }
    pgc->SetPen(dc->GetPen());
    pgc->StrokeLines(n, dPoints);
    free(dPoints);
  } else
#endif
    DrawLines(n, points, 0, 0, true);
}

void ocpnDC::DrawRectangle(wxCoord x, wxCoord y, wxCoord w, wxCoord h) {
  if (dc) dc->DrawRectangle(x, y, w, h);
}

void ocpnDC::DrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                  wxCoord r) {
  if (dc) dc->DrawRoundedRectangle(x, y, w, h, r);
}

void ocpnDC::DrawCircle(wxCoord x, wxCoord y, wxCoord radius) {
  glEnable(GL_BLEND);

  float coords[8];
  coords[0] = x - radius;
  coords[1] = y + radius;
  coords[2] = x + radius;
  coords[3] = y + radius;
  coords[4] = x - radius;
  coords[5] = y - radius;
  coords[6] = x + radius;
  coords[7] = y - radius;

  GLShaderProgram *shader;
  
  shader = m_pcircle_filled_shader_program;
  shader->Bind();
  shader->SetUniformMatrix4fv("MVMatrix", (float *)&(m_vp.vp_matrix_transform));
  
  shader->SetUniform1f("circle_radius", radius);

  //  Circle center point
  float ctrv[2];
  ctrv[0] = x;
  int width, height;
  GetSize(&width, &height);
  ctrv[1] = height - y;
  shader->SetUniform2fv("circle_center", ctrv);

    //  Circle color
  float colorv[4];
  if (m_brush.IsOk()) {
    colorv[0] = m_brush.GetColour().Red() / float(256);
    colorv[1] = m_brush.GetColour().Green() / float(256);
    colorv[2] = m_brush.GetColour().Blue() / float(256);
    colorv[3] = (m_brush.GetStyle() == wxBRUSHSTYLE_TRANSPARENT) ? 0.0 : 1.0;
  }
  shader->SetUniform4fv("circle_color", colorv);

  //  Border color
  float bcolorv[4];
  if (m_pen.IsOk()) {
    bcolorv[0] = m_pen.GetColour().Red() / float(256);
    bcolorv[1] = m_pen.GetColour().Green() / float(256);
    bcolorv[2] = m_pen.GetColour().Blue() / float(256);
    bcolorv[3] = m_pen.GetColour().Alpha() / float(256);
  }
  shader->SetUniform4fv("border_color", bcolorv);

  //  Border Width
  if (m_pen.IsOk())
    shader->SetUniform1f("border_width", m_pen.GetWidth());
  else
    shader->SetUniform1f("border_width", 2);

  shader->SetAttributePointerf("aPos", coords);

  // Perform the actual drawing.
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  shader->UnBind();
}

void ocpnDC::StrokeCircle(wxCoord x, wxCoord y, wxCoord radius) {
#if wxUSE_GRAPHICS_CONTEXT
  if (pgc) {
    wxGraphicsPath gpath = pgc->CreatePath();
    gpath.AddCircle(x, y, radius);

    pgc->SetPen(GetPen());
    pgc->SetBrush(GetBrush());
    pgc->DrawPath(gpath);

    // keep dc dirty box up-to-date
    dc->CalcBoundingBox(x + radius + 2, y + radius + 2);
    dc->CalcBoundingBox(x - radius - 2, y - radius - 2);
  } else
#endif
    DrawCircle(x, y, radius);
}

void ocpnDC::DrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height) {
  if (dc) dc->DrawEllipse(x, y, width, height);
#ifdef ocpnUSE_GL
  else {
    float r1 = width / 2, r2 = height / 2;
    float cx = x + r1, cy = y + r2;

    //      Enable anti-aliased lines, at best quality
    glEnable(GL_BLEND);

    /* formula for variable step count to produce smooth ellipse */
    float steps = floorf(
        wxMax(sqrtf(sqrtf((float)(width * width + height * height))), 1) *
        M_PI);

    // FIXME (dave??)  Unimplemented for GLSL and GLES2
    glDisable(GL_BLEND);
  }
#endif
}

void ocpnDC::DrawPolygon(int n, wxPoint points[], wxCoord xoffset,
                         wxCoord yoffset, float scale, float angle) {
  if (dc) dc->DrawPolygon(n, points, xoffset, yoffset);
}

void ocpnDC::DrawPolygonTessellated(int n, wxPoint points[], wxCoord xoffset,
                                    wxCoord yoffset) {
  if (dc) dc->DrawPolygon(n, points, xoffset, yoffset);
}

void ocpnDC::StrokePolygon(int n, wxPoint points[], wxCoord xoffset,
                           wxCoord yoffset, float scale) {
#if wxUSE_GRAPHICS_CONTEXT
  if (pgc) {
    wxGraphicsPath gpath = pgc->CreatePath();
    gpath.MoveToPoint(points[0].x * scale + xoffset,
                      points[0].y * scale + yoffset);
    for (int i = 1; i < n; i++)
      gpath.AddLineToPoint(points[i].x * scale + xoffset,
                           points[i].y * scale + yoffset);
    gpath.AddLineToPoint(points[0].x * scale + xoffset,
                         points[0].y * scale + yoffset);

    pgc->SetPen(GetPen());
    pgc->SetBrush(GetBrush());
    pgc->DrawPath(gpath);

    for (int i = 0; i < n; i++)
      dc->CalcBoundingBox(points[i].x * scale + xoffset,
                          points[i].y * scale + yoffset);
  } else
#endif
    DrawPolygon(n, points, xoffset, yoffset, scale);
}

void ocpnDC::DrawBitmap(const wxBitmap &bitmap, wxCoord x, wxCoord y,
                        bool usemask) {
  wxBitmap bmp;
  if (x < 0 || y < 0) {
    int dx = (x < 0 ? -x : 0);
    int dy = (y < 0 ? -y : 0);
    int w = bitmap.GetWidth() - dx;
    int h = bitmap.GetHeight() - dy;
    /* picture is out of viewport */
    if (w <= 0 || h <= 0) return;
    wxBitmap newBitmap = bitmap.GetSubBitmap(wxRect(dx, dy, w, h));
    x += dx;
    y += dy;
    bmp = newBitmap;
  } else {
    bmp = bitmap;
  }
  if (dc) dc->DrawBitmap(bmp, x, y, usemask);
}

void ocpnDC::DrawText(const wxString &text, wxCoord x, wxCoord y, float angle) {
  if (dc) dc->DrawText(text, x, y);
}

void ocpnDC::GetTextExtent(const wxString &string, wxCoord *w, wxCoord *h,
                           wxCoord *descent, wxCoord *externalLeading,
                           wxFont *font) {
  //  Give at least reasonable results on failure.
  if (w) *w = 100;
  if (h) *h = 100;

  if (dc)
    dc->GetTextExtent(string, w, h, descent, externalLeading, font);
  else {
    wxFont f = m_font;
    if (font) f = *font;

    //FIXME Dave Re-enable, and fix rotation logic.
    if (0/*m_buseTex*/) {
      wxMemoryDC temp_dc;
      temp_dc.GetTextExtent(string, w, h, descent, externalLeading, &f);
    } else {
      wxMemoryDC temp_dc;
      temp_dc.GetTextExtent(string, w, h, descent, externalLeading, &f);
    }
  }

  //  Sometimes GetTextExtent returns really wrong, uninitialized results.
  //  Dunno why....
  if (w && (*w > 500)) *w = 500;
  if (h && (*h > 500)) *h = 500;
}

void ocpnDC::ResetBoundingBox() {
  if (dc) dc->ResetBoundingBox();
}

void ocpnDC::CalcBoundingBox(wxCoord x, wxCoord y) {
  if (dc) dc->CalcBoundingBox(x, y);
}

bool ocpnDC::ConfigurePen() {
  if (!m_pen.IsOk()) return false;
  if (m_pen == *wxTRANSPARENT_PEN) return false;

  wxColour c = m_pen.GetColour();
  int width = m_pen.GetWidth();

  return true;
}

bool ocpnDC::ConfigureBrush() {
  if (m_brush == wxNullBrush || m_brush.GetStyle() == wxBRUSHSTYLE_TRANSPARENT)
    return false;

  return true;
}

void ocpnDC::GLDrawBlendData(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                             int format, const unsigned char *data) {
}

void ocpnDC::SetVP(ViewPort vp){
  m_vp = vp;
  m_vp.SetVPTransformMatrix();
  BuildShaders();

  // Program the matrix transforms for the several private shaders
  mat4x4 I;
  mat4x4_identity(I);


  if (m_pcolor_tri_shader_program) {
    m_pcolor_tri_shader_program->Bind();
    m_pcolor_tri_shader_program->SetUniformMatrix4fv(
        "MVMatrix", (GLfloat *)m_vp.vp_matrix_transform);
    m_pcolor_tri_shader_program->SetUniformMatrix4fv("TransformMatrix",
                                                     (GLfloat *)I);
    m_pcolor_tri_shader_program->UnBind();
  }
  if (m_pAALine_shader_program) {
    m_pAALine_shader_program->Bind();
    m_pAALine_shader_program->SetUniformMatrix4fv(
        "MVMatrix", (GLfloat *)m_vp.vp_matrix_transform);
    m_pAALine_shader_program->SetUniformMatrix4fv("TransformMatrix",
                                                  (GLfloat *)I);
    m_pAALine_shader_program->UnBind();
  }

  if (m_pcircle_filled_shader_program) {
    m_pcircle_filled_shader_program->Bind();
    m_pcircle_filled_shader_program->SetUniformMatrix4fv(
        "MVMatrix", (GLfloat *)m_vp.vp_matrix_transform);
    m_pcircle_filled_shader_program->SetUniformMatrix4fv("TransformMatrix",
                                                         (GLfloat *)I);
    m_pcircle_filled_shader_program->UnBind();
  }

  if (m_ptexture_2D_shader_program) {
    m_ptexture_2D_shader_program->Bind();
    m_ptexture_2D_shader_program->SetUniformMatrix4fv(
        "MVMatrix", (GLfloat *)m_vp.vp_matrix_transform);
    m_ptexture_2D_shader_program->SetUniformMatrix4fv("TransformMatrix",
                                                      (GLfloat *)I);
    m_ptexture_2D_shader_program->UnBind();
  }
}

const GLchar* odc_preamble =
"#version 120\n"
"#define precision\n"
"#define lowp\n"
"#define mediump\n"
"#define highp\n";

// Simple colored triangle shader

static const GLchar* odc_color_tri_vertex_shader_source =
    "attribute vec2 position;\n"
    "uniform mat4 MVMatrix;\n"
    "uniform mat4 TransformMatrix;\n"
    "uniform vec4 color;\n"
    "varying vec4 fragColor;\n"
    "void main() {\n"
    "   fragColor = color;\n"
    "   gl_Position = MVMatrix * TransformMatrix * vec4(position, 0.0, 1.0);\n"
    "}\n";

static const GLchar* odc_color_tri_fragment_shader_source =
    "precision lowp float;\n"
    "varying vec4 fragColor;\n"
    "void main() {\n"
    "   gl_FragColor = fragColor;\n"
    "}\n";

static const GLchar* odc_AALine_vertex_shader_source =
    "uniform vec2 uViewPort; //Width and Height of the viewport\n"
    "varying vec2 vLineCenter;\n"
    "attribute vec2 position;\n"
    "uniform mat4 MVMatrix;\n"
    "uniform mat4 TransformMatrix;\n"
  "void main()\n"
  "{\n"
  "   vec4 pp = MVMatrix * vec4(position, 0.0, 1.0);\n"
  "   gl_Position = pp;\n"
  "   vec2 vp = uViewPort;\n"
  "   vLineCenter = 0.5*(pp.xy + vec2(1, 1))*vp;\n"
  "}\n";


static const GLchar* odc_AALine_fragment_shader_source =
    "precision mediump float;\n"
    "uniform float uLineWidth;\n"
    "uniform vec4 color;\n"
    "uniform float uBlendFactor; //1.5..2.5\n"
    "varying vec2 vLineCenter;\n"
    "void main()\n"
    "{\n"
    "    vec4 col = color;\n"
    "    float d = length(vLineCenter-gl_FragCoord.xy);\n"
    "    float w = uLineWidth;\n"
    "    if (d>w)\n"
    "      col.w = 0.0;\n"
    "    else{\n"
    "      if(float((w/2-d)/(w/2)) < .5){\n"
    "        //col.w *= pow(float((w-d)/w), uBlendFactor);\n"
    "        col.w *= pow(float((w/2-d)/(w/2)), uBlendFactor);\n"
    "      }\n"
    "    }\n"
    "    gl_FragColor = col;\n"
    "}\n";

//  Circle shader
static const GLchar* odc_circle_filled_vertex_shader_source =
    "precision highp float;\n"
    "attribute vec2 aPos;\n"
    "uniform mat4 MVMatrix;\n"
    "uniform mat4 TransformMatrix;\n"
    "void main() {\n"
    "   gl_Position = MVMatrix * TransformMatrix * vec4(aPos, 0.0, 1.0);\n"
    "}\n";

static const GLchar* odc_circle_filled_fragment_shader_source =
    "precision highp float;\n"
    "uniform float border_width;\n"
    "uniform float circle_radius;\n"
    "uniform vec4 circle_color;\n"
    "uniform vec4 border_color;\n"
    "uniform vec2 circle_center;\n"
    "void main(){\n"
    "float d = distance(gl_FragCoord.xy, circle_center);\n"
    "if (d < (circle_radius - border_width)) { gl_FragColor = circle_color; }\n"
    "else if (d < circle_radius) { gl_FragColor = border_color; }\n"
    "else { gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0); }\n"
    "}\n";

// Simple 2D texture shader
static const GLchar* odc_texture_2D_vertex_shader_source =
    "attribute vec2 aPos;\n"
    "attribute vec2 aUV;\n"
    "uniform mat4 MVMatrix;\n"
    "uniform mat4 TransformMatrix;\n"
    "varying vec2 varCoord;\n"
    "void main() {\n"
    "   gl_Position = MVMatrix * TransformMatrix * vec4(aPos, 0.0, 1.0);\n"
    "   varCoord = aUV;\n"
    "}\n";

static const GLchar* odc_texture_2D_fragment_shader_source =
    "precision lowp float;\n"
    "uniform sampler2D uTex;\n"
    "varying vec2 varCoord;\n"
    "void main() {\n"
    "   gl_FragColor = texture2D(uTex, varCoord);\n"
    "}\n";

void ocpnDC::BuildShaders(){

  // Simple colored triangle shader
  if (!m_pcolor_tri_shader_program) {
    GLShaderProgram *shaderProgram = new GLShaderProgram;
    shaderProgram->addShaderFromSource(odc_color_tri_vertex_shader_source, GL_VERTEX_SHADER);
    shaderProgram->addShaderFromSource(odc_color_tri_fragment_shader_source, GL_FRAGMENT_SHADER);
    shaderProgram->linkProgram();

    if (shaderProgram->isOK())
      m_pcolor_tri_shader_program = shaderProgram;
  }

  if (!m_pAALine_shader_program) {
    GLShaderProgram *shaderProgram = new GLShaderProgram;
    shaderProgram->addShaderFromSource(odc_AALine_fragment_shader_source, GL_FRAGMENT_SHADER);
    shaderProgram->addShaderFromSource(odc_AALine_vertex_shader_source, GL_VERTEX_SHADER);
    shaderProgram->linkProgram();

    if (shaderProgram->isOK())
      m_pAALine_shader_program = shaderProgram;
  }

  if (!m_pcircle_filled_shader_program) {
    GLShaderProgram *shaderProgram = new GLShaderProgram;
    shaderProgram->addShaderFromSource(odc_circle_filled_vertex_shader_source, GL_VERTEX_SHADER);
    shaderProgram->addShaderFromSource(odc_circle_filled_fragment_shader_source, GL_FRAGMENT_SHADER);
    shaderProgram->linkProgram();

    if (shaderProgram->isOK())
      m_pcircle_filled_shader_program = shaderProgram;
  }

  if (!m_ptexture_2D_shader_program) {
    GLShaderProgram *shaderProgram = new GLShaderProgram;
    shaderProgram->addShaderFromSource(odc_texture_2D_vertex_shader_source, GL_VERTEX_SHADER);
    shaderProgram->addShaderFromSource(odc_texture_2D_fragment_shader_source, GL_FRAGMENT_SHADER);
    shaderProgram->linkProgram();

    if (shaderProgram->isOK())
      m_ptexture_2D_shader_program = shaderProgram;
  }

}
