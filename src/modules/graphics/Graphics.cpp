#include "modules/graphics/Graphics.h"
#include "modules/graphics/SpriteRenderer.h"
#include "modules/graphics/GTxt.h"
#include "modules/graphics/DTex.h"
#include "modules/graphics/LoadingList.h"
#include "modules/render/Render.h"

#include <unirender/Context.h>
#include <unirender/ShaderProgram.h>
#include <unirender/RenderState.h>
#include <unirender/Factory.h>

#include <string.h>

namespace tt
{

TT_SINGLETON_DEFINITION(Graphics)

Graphics::Graphics()
{
    auto dev = tt::Render::Instance()->Device();
    DTex::Instance()->Init(*dev);
    GTxt::Instance()->Init(*dev);

    auto filtpath = "assets\\fonts\\default2.ttf";
    GTxt::Instance()->LoadFonts({ { "default", filtpath } }, { {} });

    m_spr_rd = std::make_shared<tt::SpriteRenderer>();
}

Graphics::~Graphics()
{
}

void Graphics::OnSize(float width, float height)
{
    auto ctx = tt::Render::Instance()->Context();
    ctx->SetViewport(0, 0, (int)width, (int)height);

    m_spr_rd->OnSize(width, height);
}

void Graphics::OnCameraUpdate(const sm::vec2& offset, float scale)
{
    m_spr_rd->OnCameraUpdate(offset, scale);
}

void Graphics::DrawPainter(const tess::Painter& pt) const
{
    m_spr_rd->DrawPainter(*Render::Instance()->Context(), ur::DefaultRenderState2D(), pt);
}

void Graphics::DrawText(const char* text, const sm::Matrix2D& mt, const gtxt_label_style& style) const
{
    GTxt::Draw(*Render::Instance()->Context(), text, style, mt, 0xffffffff, 0, 0, false);
}

void Graphics::DrawTexQuad(const float* positions, const float* texcoords,
                           const ur::TexturePtr& tex, uint32_t color)
{
    m_spr_rd->DrawQuad(*Render::Instance()->Context(), ur::DefaultRenderState2D(), positions, texcoords, tex, color);
}

void Graphics::Flush()
{
    auto ctx = Render::Instance()->Context();

    DTex::Instance()->Flush(*ctx);
    LoadingList::Instance()->Flush(*ctx);

    m_spr_rd->Flush(*ctx);
}

}