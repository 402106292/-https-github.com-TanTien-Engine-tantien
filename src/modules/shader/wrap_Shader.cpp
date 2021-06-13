#include "modules/shader/wrap_Shader.h"
#include "modules/script/Proxy.h"

#include <shadertrans/ShaderStage.h>
#include <shadertrans/ShaderTrans.h>
#include <shadertrans/SpirvTools.h>
#include <shadertrans/ShaderReflection.h>
#include <shadertrans/spirv_Linker.h>
#include <shadertrans/ShaderBuilder.h>
#include <shadertrans/SpirvGenTwo.h>
#include <shadertrans/ShaderPreprocess.h>
#include <vessel/src/value.h>
#include <guard/check.h>

#include <vector>

#include <string.h>

namespace
{

double pointer2double(void* pointer)
{
    return (double)(uintptr_t)(pointer);
}

void* double2pointer(double d)
{
    return (void*)(uintptr_t)(d);
}

void return_pointer(void* ptr)
{
    if (ptr) {
        ves_set_number(0, pointer2double(ptr));
    } else {
        ves_set_nil(0);
    }
}

shadertrans::ShaderStage to_shader_stage(const char* str)
{
    shadertrans::ShaderStage stage;
    if (strcmp(str, "vertex") == 0) {
        stage = shadertrans::ShaderStage::VertexShader;
    } else if (strcmp(str, "tess_ctrl") == 0) {
        stage = shadertrans::ShaderStage::TessCtrlShader;
    } else if (strcmp(str, "tess_eval") == 0) {
        stage = shadertrans::ShaderStage::TessEvalShader;
    } else if (strcmp(str, "geometry") == 0) {
        stage = shadertrans::ShaderStage::GeometryShader;
    } else if (strcmp(str, "pixel") == 0) {
        stage = shadertrans::ShaderStage::PixelShader;
    } else if (strcmp(str, "compute") == 0) {
        stage = shadertrans::ShaderStage::ComputeShader;
    }
    return stage;
}

// ShaderTools

void w_ShaderTools_code2spirv()
{
    const char* stage_str = ves_tostring(1);
    const char* code_str = ves_tostring(2);
    const char* lang_str = ves_tostring(3);
    
    std::vector<unsigned int> spirv;
    auto stage = to_shader_stage(stage_str);
    if (strcmp(lang_str, "glsl") == 0) {
        shadertrans::ShaderTrans::GLSL2SpirV(stage, code_str, spirv, true);
    } else if (strcmp(lang_str, "hlsl") == 0) {
        shadertrans::ShaderTrans::HLSL2SpirV(stage, code_str, "main", spirv);
    }

    ves_pop(4);

    ves_newlist((int)spirv.size());
    for (int i = 0; i < spirv.size(); ++i)
    {
        ves_pushnumber(spirv[i]);
        ves_seti(-2, i);
        ves_pop(1);
    }
}

void w_ShaderTools_disassemble()
{
    const int len = ves_len(1);
    std::vector<unsigned int> spirv(len);
    for (int i = 0; i < len; ++i)
    {
        ves_geti(1, i);
        spirv[i] = (unsigned int)ves_tonumber(-1);
        ves_pop(1);
    }

    std::string text;
    shadertrans::SpirvTools::Disassemble(spirv.data(), spirv.size(), &text);
    ves_set_lstring(0, text.c_str(), text.size());
}

void w_ShaderTools_hlsl2glsl()
{
    const char* stage_str = ves_tostring(1);
    const char* hlsl = ves_tostring(2);
    const char* entry_point = ves_tostring(3);

    std::vector<unsigned int> spirv;
    auto stage = to_shader_stage(stage_str);
    shadertrans::ShaderTrans::HLSL2SpirV(stage, hlsl, entry_point, spirv);
    std::string glsl;
    shadertrans::ShaderTrans::SpirV2GLSL(stage, spirv, glsl);
    glsl = shadertrans::ShaderPreprocess::PrepareHLSL(glsl, entry_point);

    ves_set_lstring(0, glsl.c_str(), glsl.size());
}

// SpirvGenTwo

void w_SpirvGenTwo_get_type()
{
    spvgentwo::Instruction* inst = (spvgentwo::Instruction*)double2pointer(ves_tonumber(1));
    if (!inst) {
        ves_set_nil(0);
    } else {
        const char* type = shadertrans::SpirvGenTwo::GetType(*inst);
        ves_set_lstring(0, type, strlen(type));
    }
}

void w_SpirvGenTwo_is_vector()
{
    spvgentwo::Instruction* inst = (spvgentwo::Instruction*)double2pointer(ves_tonumber(1));
    if (!inst) {
        ves_set_nil(0);
    } else {
        ves_set_boolean(0, shadertrans::SpirvGenTwo::IsVector(*inst));
    }
}

void w_SpirvGenTwo_get_vector_num()
{
    spvgentwo::Instruction* inst = (spvgentwo::Instruction*)double2pointer(ves_tonumber(1));
    if (!inst) {
        ves_set_nil(0);
    } else {
        ves_set_number(0, shadertrans::SpirvGenTwo::GetVectorNum(*inst));
    }
}

void w_SpirvGenTwo_add()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* a = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* b = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::Add(func, a, b);
    return_pointer(ret);
}

void w_SpirvGenTwo_sub()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* a = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* b = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::Sub(func, a, b);
    return_pointer(ret);
}

void w_SpirvGenTwo_mix()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* x = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* y = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));
    spvgentwo::Instruction* a = (spvgentwo::Instruction*)double2pointer(ves_tonumber(4));

    auto ret = shadertrans::SpirvGenTwo::Mix(func, x, y, a);
    return_pointer(ret);
}

void w_SpirvGenTwo_pow()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* x = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* y = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::Pow(func, x, y);
    return_pointer(ret);
}

void w_SpirvGenTwo_sqrt()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* v = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));

    auto ret = shadertrans::SpirvGenTwo::Sqrt(func, v);
    return_pointer(ret);
}

void w_SpirvGenTwo_dot()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* a = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* b = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::Dot(func, a, b);
    return_pointer(ret);
}

void w_SpirvGenTwo_cross()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* a = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* b = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::Cross(func, a, b);
    return_pointer(ret);
}

void w_SpirvGenTwo_negate()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* v = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));

    auto ret = shadertrans::SpirvGenTwo::Negate(func, v);
    return_pointer(ret);
}

void w_SpirvGenTwo_add_variable()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    const char* name = ves_tostring(2);
    spvgentwo::Instruction* value = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::AddVariable(func, name, value);
    return_pointer(ret);
}

// ShaderGen

void w_ShaderGen_allocate()
{
    auto builder = std::make_shared<shadertrans::ShaderBuilder>();
    auto proxy = (tt::Proxy<shadertrans::ShaderBuilder>*)ves_set_newforeign(0, 0, sizeof(tt::Proxy<shadertrans::ShaderBuilder>));
    proxy->obj = builder;
}

int w_ShaderGen_finalize(void* data)
{
    auto proxy = (tt::Proxy<shadertrans::ShaderBuilder>*)(data);
    proxy->~Proxy();
    return sizeof(tt::Proxy<shadertrans::ShaderBuilder>);
}

void w_ShaderGen_add_input()
{
    const char* name = ves_tostring(1);
    const char* type = ves_tostring(2);

    auto builder = ((tt::Proxy<shadertrans::ShaderBuilder>*)ves_toforeign(0))->obj;
    auto input = builder->AddInput(name, type);
    return_pointer(input);
}

void w_ShaderGen_add_output()
{
    const char* name = ves_tostring(1);
    const char* type = ves_tostring(2);

    auto builder = ((tt::Proxy<shadertrans::ShaderBuilder>*)ves_toforeign(0))->obj;
    auto output = builder->AddOutput(name, type);
    return_pointer(output);
}

void w_ShaderGen_add_uniform()
{
    spvgentwo::Module* module = (spvgentwo::Module*)double2pointer(ves_tonumber(1));
    const char* name = ves_tostring(2);
    const char* type = ves_tostring(3);

    auto builder = ((tt::Proxy<shadertrans::ShaderBuilder>*)ves_toforeign(0))->obj;
    auto unif = builder->AddUniform(module, name, type);
    return_pointer(unif);
}

void w_ShaderGen_query_unif_name()
{
    spvgentwo::Instruction* unif = (spvgentwo::Instruction*)double2pointer(ves_tonumber(1));

    auto builder = ((tt::Proxy<shadertrans::ShaderBuilder>*)ves_toforeign(0))->obj;
    auto name = builder->QueryUniformName(unif);
    ves_set_lstring(0, name, strlen(name));
}

void w_ShaderGen_access_chain()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* base = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    int index = (int)ves_tonumber(3);

    auto ret = shadertrans::SpirvGenTwo::AccessChain(func, base, index);
    return_pointer(ret);
}

void w_shadergen_compose_float2()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* x = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* y = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::ComposeFloat2(func, x, y);
    return_pointer(ret);
}

void w_shadergen_compose_float3()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* x = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* y = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));
    spvgentwo::Instruction* z = (spvgentwo::Instruction*)double2pointer(ves_tonumber(4));

    auto ret = shadertrans::SpirvGenTwo::ComposeFloat3(func, x, y, z);
    return_pointer(ret);
}

void w_shadergen_compose_float4()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* x = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* y = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));
    spvgentwo::Instruction* z = (spvgentwo::Instruction*)double2pointer(ves_tonumber(4));
    spvgentwo::Instruction* w = (spvgentwo::Instruction*)double2pointer(ves_tonumber(5));

    auto ret = shadertrans::SpirvGenTwo::ComposeFloat4(func, x, y, z, w);
    return_pointer(ret);
}

void w_shadergen_compose_extract()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* comp = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    int index = (int)ves_tonumber(3);

    auto ret = shadertrans::SpirvGenTwo::ComposeExtract(func, comp, index);
    return_pointer(ret);
}

void w_shadergen_mul()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* a = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* b = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::Mul(func, a, b);
    return_pointer(ret);
}

void w_shadergen_div()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* a = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* b = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::Div(func, a, b);
    return_pointer(ret);
}

void w_shadergen_reflect()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* I = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* N = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::Reflect(func, I, N);
    return_pointer(ret);
}

void w_shadergen_normalize()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* v = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));

    auto ret = shadertrans::SpirvGenTwo::Normalize(func, v);
    return_pointer(ret);
}

void w_shadergen_length()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* v = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));

    auto ret = shadertrans::SpirvGenTwo::Length(func, v);
    return_pointer(ret);
}

void w_shadergen_max()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* x = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* y = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::Max(func, x, y);
    return_pointer(ret);
}

void w_shadergen_min()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* x = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* y = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::Min(func, x, y);
    return_pointer(ret);
}

void w_shadergen_clamp()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* x = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* min = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));
    spvgentwo::Instruction* max = (spvgentwo::Instruction*)double2pointer(ves_tonumber(4));

    auto ret = shadertrans::SpirvGenTwo::Clamp(func, x, min, max);
    if (!ret) {
        ves_set_nil(0);
    } else {
        return_pointer(ret);
    }
}

void w_ShaderGen_store()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* dst = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* src = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    shadertrans::SpirvGenTwo::Store(func, dst, src);
}

void w_ShaderGen_load()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* val = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));

    auto ret = shadertrans::SpirvGenTwo::Load(func, val);
    return_pointer(ret);
}

void w_ShaderGen_image_sample()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* img = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* uv = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));
    spvgentwo::Instruction* lod = (spvgentwo::Instruction*)double2pointer(ves_tonumber(4));

    auto ret = shadertrans::SpirvGenTwo::ImageSample(func, img, uv, lod);
    return_pointer(ret);
}

void w_ShaderGen_add_module()
{
    const char* stage_str = ves_tostring(1);
    const char* code_str = ves_tostring(2);
    const char* lang_str = ves_tostring(3);
    const char* name = ves_tostring(4);
    const char* enter_point = ves_tostring(5);

    auto stage = to_shader_stage(stage_str);

    auto builder = ((tt::Proxy<shadertrans::ShaderBuilder>*)ves_toforeign(0))->obj;
    auto lib = builder->AddModule(stage, code_str, lang_str, name, enter_point)->impl;
    return_pointer(lib.get());
}

void w_ShaderGen_query_func()
{
    spvgentwo::Module* module = (spvgentwo::Module*)double2pointer(ves_tonumber(1));
    const char* name = ves_tostring(2);

    auto func = shadertrans::SpirvGenTwo::QueryFunction(*module, name);
    return_pointer(func);
}

void w_ShaderGen_func_replace()
{
    auto builder = ((tt::Proxy<shadertrans::ShaderBuilder>*)ves_toforeign(0))->obj;
    spvgentwo::Function* from = (spvgentwo::Function*)double2pointer(ves_tonumber(1));
    spvgentwo::Function* to = (spvgentwo::Function*)double2pointer(ves_tonumber(2));

    builder->ReplaceFunc(from, to);
}

void w_ShaderGen_get_main_module()
{
    auto builder = ((tt::Proxy<shadertrans::ShaderBuilder>*)ves_toforeign(0))->obj;
    spvgentwo::Module* main = builder->GetMainModule();
    return_pointer(main);
}

void w_ShaderGen_get_main_func()
{
    auto builder = ((tt::Proxy<shadertrans::ShaderBuilder>*)ves_toforeign(0))->obj;
    spvgentwo::Function* main = builder->GetMainFunc();
    return_pointer(main);
}

void w_ShaderGen_get_func_block()
{
    spvgentwo::Function* func = (spvgentwo::Function*)double2pointer(ves_tonumber(1));
    spvgentwo::BasicBlock* bb = shadertrans::SpirvGenTwo::GetFuncBlock(func);
    return_pointer(bb);
}

void w_ShaderGen_create_decl_func()
{
    spvgentwo::Module* module = (spvgentwo::Module*)double2pointer(ves_tonumber(1));
    spvgentwo::Function* func = (spvgentwo::Function*)double2pointer(ves_tonumber(2));

    auto ret = shadertrans::SpirvGenTwo::CreateDeclFunc(module, func);
    return_pointer(ret);
}

void w_ShaderGen_add_link_decl()
{
    auto builder = ((tt::Proxy<shadertrans::ShaderBuilder>*)ves_toforeign(0))->obj;
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    const char* name = ves_tostring(2);
    bool is_export = ves_toboolean(3);

    builder->AddLinkDecl(func, name, is_export);
}

void w_ShaderGen_create_func()
{
    spvgentwo::Module* module = (spvgentwo::Module*)double2pointer(ves_tonumber(1));
    const char* name = ves_tostring(2);
    const char* ret = ves_tostring(3);

    const int len = ves_len(4);
    std::vector<std::string> args(len);
    for (int i = 0; i < len; ++i)
    {
        ves_geti(4, i);
        args[i] = ves_tostring(-1);
        ves_pop(1);
    }

    auto func = shadertrans::SpirvGenTwo::CreateFunc(module, name, ret, args);
    return_pointer(func);
}

void w_ShaderGen_get_func_param()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    int index = (int)ves_tonumber(2);

    auto ret = shadertrans::SpirvGenTwo::GetFuncParam(func, index);
    return_pointer(ret);
}

void w_ShaderGen_get_func_args()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));

    std::vector<std::string> names;
    shadertrans::SpirvGenTwo::GetFuncParamNames(func, names);

    ves_pop(2);

    ves_newlist((int)names.size());
    for (int i = 0; i < names.size(); ++i)
    {
        ves_pushstring(names[i].c_str());
        ves_seti(-2, i);
        ves_pop(1);
    }
}

void w_ShaderGen_func_call()
{
    spvgentwo::Function* caller = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Function* callee = (spvgentwo::Function*)(double2pointer(ves_tonumber(2)));

    const int len = ves_len(3);
    std::vector<spvgentwo::Instruction*> args(len);
    for (int i = 0; i < len; ++i)
    {
        ves_geti(3, i);
        args[i] = (spvgentwo::Instruction*)(double2pointer(ves_tonumber(-1)));
        ves_pop(1);
    }

    for (auto& arg : args) {
        if (!arg) {
            ves_set_nil(0);
            return;
        }
    }

    auto ret = shadertrans::SpirvGenTwo::FuncCall(caller, callee, args);
    return_pointer(ret);
}

void w_ShaderGen_func_return()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));

    shadertrans::SpirvGenTwo::Return(func);
}

void w_shadergen_if_block()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* cond = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::BasicBlock* t = (spvgentwo::BasicBlock*)double2pointer(ves_tonumber(3));
    spvgentwo::BasicBlock* f = (spvgentwo::BasicBlock*)double2pointer(ves_tonumber(4));

    auto ret = shadertrans::SpirvGenTwo::If(func, cond, t, f);
    return_pointer(ret);
}

void w_ShaderGen_variable_float()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));

    auto ret = shadertrans::SpirvGenTwo::VariableFloat(func);
    return_pointer(ret);
}

void w_ShaderGen_variable_float2()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));

    auto ret = shadertrans::SpirvGenTwo::VariableFloat2(func);
    return_pointer(ret);
}

void w_ShaderGen_variable_float3()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));

    auto ret = shadertrans::SpirvGenTwo::VariableFloat3(func);
    return_pointer(ret);
}

void w_ShaderGen_variable_float4()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));

    auto ret = shadertrans::SpirvGenTwo::VariableFloat4(func);
    return_pointer(ret);
}

void w_ShaderGen_add_block()
{
    spvgentwo::Function* func = (spvgentwo::Function*)(double2pointer(ves_tonumber(1)));
    const char* name = ves_tostring(2);

    auto ret = shadertrans::SpirvGenTwo::AddBlock(func, name);
    return_pointer(ret);
}

void w_ShaderGen_const_bool()
{
    spvgentwo::Module* module = (spvgentwo::Module*)double2pointer(ves_tonumber(1));
    bool b = ves_toboolean(2);

    auto ret = shadertrans::SpirvGenTwo::ConstBool(module, b);
    return_pointer(ret);
}

void w_ShaderGen_const_int()
{
    spvgentwo::Module* module = (spvgentwo::Module*)double2pointer(ves_tonumber(1));
    int x = (int)ves_tonumber(2);

    auto ret = shadertrans::SpirvGenTwo::ConstInt(module, x);
    return_pointer(ret);
}

void w_ShaderGen_const_float()
{
    spvgentwo::Module* module = (spvgentwo::Module*)double2pointer(ves_tonumber(1));
    float x = (float)ves_tonumber(2);
    
    auto ret = shadertrans::SpirvGenTwo::ConstFloat(module, x);
    return_pointer(ret);
}

void w_ShaderGen_const_float2()
{
    spvgentwo::Module* module = (spvgentwo::Module*)double2pointer(ves_tonumber(1));
    float x = (float)ves_tonumber(2);
    float y = (float)ves_tonumber(3);

    auto ret = shadertrans::SpirvGenTwo::ConstFloat2(module, x, y);
    return_pointer(ret);
}

void w_ShaderGen_const_float3()
{
    spvgentwo::Module* module = (spvgentwo::Module*)double2pointer(ves_tonumber(1));
    float x = (float)ves_tonumber(2);
    float y = (float)ves_tonumber(3);
    float z = (float)ves_tonumber(4);

    auto ret = shadertrans::SpirvGenTwo::ConstFloat3(module, x, y, z);
    return_pointer(ret);
}

void w_ShaderGen_const_float4()
{
    spvgentwo::Module* module = (spvgentwo::Module*)double2pointer(ves_tonumber(1));
    float x = (float)ves_tonumber(2);
    float y = (float)ves_tonumber(3);
    float z = (float)ves_tonumber(4);
    float w = (float)ves_tonumber(5);

    auto ret = shadertrans::SpirvGenTwo::ConstFloat4(module, x, y, z, w);
    return_pointer(ret);
}

void w_ShaderGen_const_mat2()
{
    spvgentwo::Module* module = (spvgentwo::Module*)double2pointer(ves_tonumber(1));

    float m[4];
    const int len = ves_len(2);
    GD_ASSERT(len == 4, "error m size");
    std::vector<unsigned int> spirv(len);
    for (int i = 0; i < len; ++i)
    {
        ves_geti(2, i);
        m[i] = (float)ves_tonumber(-1);
        ves_pop(1);
    }

    auto ret = shadertrans::SpirvGenTwo::ConstMatrix2(module, m);
    return_pointer(ret);
}

void w_ShaderGen_const_mat3()
{
    spvgentwo::Module* module = (spvgentwo::Module*)double2pointer(ves_tonumber(1));

    float m[9];
    const int len = ves_len(2);
    GD_ASSERT(len == 9, "error m size");
    std::vector<unsigned int> spirv(len);
    for (int i = 0; i < len; ++i)
    {
        ves_geti(2, i);
        m[i] = (float)ves_tonumber(-1);
        ves_pop(1);
    }

    auto ret = shadertrans::SpirvGenTwo::ConstMatrix3(module, m);
    return_pointer(ret);
}

void w_ShaderGen_const_mat4()
{
    spvgentwo::Module* module = (spvgentwo::Module*)double2pointer(ves_tonumber(1));

    float m[16];
    const int len = ves_len(2);
    GD_ASSERT(len == 16, "error m size");
    std::vector<unsigned int> spirv(len);
    for (int i = 0; i < len; ++i)
    {
        ves_geti(2, i);
        m[i] = (float)ves_tonumber(-1);
        ves_pop(1);
    }

    auto ret = shadertrans::SpirvGenTwo::ConstMatrix4(module, m);
    return_pointer(ret);
}

void w_ShaderGen_connect_cs_main()
{
    auto builder = ((tt::Proxy<shadertrans::ShaderBuilder>*)ves_toforeign(0))->obj;
    const char* main_glsl = ves_tostring(1);
    auto glsl = builder->ConnectCSMain(main_glsl);
    ves_set_lstring(0, glsl.c_str(), glsl.size());
}

// block

void w_shadergen_is_equal()
{
    spvgentwo::BasicBlock* bb = (spvgentwo::BasicBlock*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* a = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* b = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::IsEqual(bb, a, b);
    return_pointer(ret);
}

void w_shadergen_is_not_equal()
{
    spvgentwo::BasicBlock* bb = (spvgentwo::BasicBlock*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* a = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* b = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::IsNotEqual(bb, a, b);
    return_pointer(ret);
}

void w_shadergen_is_greater()
{
    spvgentwo::BasicBlock* bb = (spvgentwo::BasicBlock*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* a = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* b = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::IsGreater(bb, a, b);
    return_pointer(ret);
}

void w_shadergen_is_greater_equal()
{
    spvgentwo::BasicBlock* bb = (spvgentwo::BasicBlock*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* a = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* b = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::IsGreaterEqual(bb, a, b);
    return_pointer(ret);
}

void w_shadergen_is_less()
{
    spvgentwo::BasicBlock* bb = (spvgentwo::BasicBlock*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* a = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* b = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::IsLess(bb, a, b);
    return_pointer(ret);
}

void w_shadergen_is_less_equal()
{
    spvgentwo::BasicBlock* bb = (spvgentwo::BasicBlock*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* a = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* b = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    auto ret = shadertrans::SpirvGenTwo::IsLessEqual(bb, a, b);
    return_pointer(ret);
}

void w_ShaderGen_kill()
{
    spvgentwo::BasicBlock* bb = (spvgentwo::BasicBlock*)(double2pointer(ves_tonumber(1)));

    shadertrans::SpirvGenTwo::Kill(bb);
}

void w_ShaderGen_bb_return()
{
    spvgentwo::BasicBlock* bb = (spvgentwo::BasicBlock*)(double2pointer(ves_tonumber(1)));

    shadertrans::SpirvGenTwo::Return(bb);
}

void w_ShaderGen_bb_return_value()
{
    spvgentwo::BasicBlock* bb = (spvgentwo::BasicBlock*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* inst = (spvgentwo::Instruction*)(double2pointer(ves_tonumber(2)));

    shadertrans::SpirvGenTwo::ReturnValue(bb, inst);
}

void w_ShaderGen_bb_store()
{
    spvgentwo::BasicBlock* bb = (spvgentwo::BasicBlock*)(double2pointer(ves_tonumber(1)));
    spvgentwo::Instruction* dst = (spvgentwo::Instruction*)double2pointer(ves_tonumber(2));
    spvgentwo::Instruction* src = (spvgentwo::Instruction*)double2pointer(ves_tonumber(3));

    shadertrans::SpirvGenTwo::Store(bb, dst, src);
}

}

namespace tt
{

VesselForeignMethodFn ShaderBindMethod(const char* signature)
{
    // ShaderTools

    if (strcmp(signature, "static ShaderTools.code2spirv(_,_,_)") == 0) return w_ShaderTools_code2spirv;
    if (strcmp(signature, "static ShaderTools.disassemble(_)") == 0) return w_ShaderTools_disassemble;

    if (strcmp(signature, "static ShaderTools.hlsl2glsl(_,_,_)") == 0) return w_ShaderTools_hlsl2glsl;

    // SpirvGenTwo

    if (strcmp(signature, "static SpirvGenTwo.get_type(_)") == 0) return w_SpirvGenTwo_get_type;
    if (strcmp(signature, "static SpirvGenTwo.is_vector(_)") == 0) return w_SpirvGenTwo_is_vector;
    if (strcmp(signature, "static SpirvGenTwo.get_vector_num(_)") == 0) return w_SpirvGenTwo_get_vector_num;

    if (strcmp(signature, "static SpirvGenTwo.add(_,_,_)") == 0) return w_SpirvGenTwo_add;
    if (strcmp(signature, "static SpirvGenTwo.sub(_,_,_)") == 0) return w_SpirvGenTwo_sub;
    if (strcmp(signature, "static SpirvGenTwo.mix(_,_,_,_)") == 0) return w_SpirvGenTwo_mix;
    if (strcmp(signature, "static SpirvGenTwo.pow(_,_,_)") == 0) return w_SpirvGenTwo_pow;
    if (strcmp(signature, "static SpirvGenTwo.sqrt(_,_)") == 0) return w_SpirvGenTwo_sqrt;
    if (strcmp(signature, "static SpirvGenTwo.dot(_,_,_)") == 0) return w_SpirvGenTwo_dot;
    if (strcmp(signature, "static SpirvGenTwo.cross(_,_,_)") == 0) return w_SpirvGenTwo_cross;
    if (strcmp(signature, "static SpirvGenTwo.negate(_,_)") == 0) return w_SpirvGenTwo_negate;
    if (strcmp(signature, "static SpirvGenTwo.add_variable(_,_,_)") == 0) return w_SpirvGenTwo_add_variable;

    // ShaderGen

    if (strcmp(signature, "ShaderGen.add_input(_,_)") == 0) return w_ShaderGen_add_input;
    if (strcmp(signature, "ShaderGen.add_output(_,_)") == 0) return w_ShaderGen_add_output;
    if (strcmp(signature, "ShaderGen.add_uniform(_,_,_)") == 0) return w_ShaderGen_add_uniform;
    if (strcmp(signature, "ShaderGen.query_unif_name(_)") == 0) return w_ShaderGen_query_unif_name;

    if (strcmp(signature, "ShaderGen.access_chain(_,_,_)") == 0) return w_ShaderGen_access_chain;
    if (strcmp(signature, "ShaderGen.compose_float2(_,_,_)") == 0) return w_shadergen_compose_float2;
    if (strcmp(signature, "ShaderGen.compose_float3(_,_,_,_)") == 0) return w_shadergen_compose_float3;
    if (strcmp(signature, "ShaderGen.compose_float4(_,_,_,_,_)") == 0) return w_shadergen_compose_float4;
    if (strcmp(signature, "ShaderGen.compose_extract(_,_,_)") == 0) return w_shadergen_compose_extract;
    if (strcmp(signature, "ShaderGen.mul(_,_,_)") == 0) return w_shadergen_mul;
    if (strcmp(signature, "ShaderGen.div(_,_,_)") == 0) return w_shadergen_div;
    if (strcmp(signature, "ShaderGen.reflect(_,_,_)") == 0) return w_shadergen_reflect;
    if (strcmp(signature, "ShaderGen.normalize(_,_)") == 0) return w_shadergen_normalize;
    if (strcmp(signature, "ShaderGen.length(_,_)") == 0) return w_shadergen_length;
    if (strcmp(signature, "ShaderGen.max(_,_,_)") == 0) return w_shadergen_max;
    if (strcmp(signature, "ShaderGen.min(_,_,_)") == 0) return w_shadergen_min;
    if (strcmp(signature, "ShaderGen.clamp(_,_,_,_)") == 0) return w_shadergen_clamp;
    if (strcmp(signature, "ShaderGen.store(_,_,_)") == 0) return w_ShaderGen_store;
    if (strcmp(signature, "ShaderGen.load(_,_)") == 0) return w_ShaderGen_load;
    if (strcmp(signature, "ShaderGen.image_sample(_,_,_,_)") == 0) return w_ShaderGen_image_sample;

    if (strcmp(signature, "ShaderGen.add_module(_,_,_,_,_)") == 0) return w_ShaderGen_add_module;
    if (strcmp(signature, "ShaderGen.query_func(_,_)") == 0) return w_ShaderGen_query_func;

    if (strcmp(signature, "ShaderGen.func_replace(_,_)") == 0) return w_ShaderGen_func_replace;

    if (strcmp(signature, "ShaderGen.get_main_module()") == 0) return w_ShaderGen_get_main_module;
    if (strcmp(signature, "ShaderGen.get_main_func()") == 0) return w_ShaderGen_get_main_func;
    if (strcmp(signature, "ShaderGen.get_func_block(_)") == 0) return w_ShaderGen_get_func_block;
    if (strcmp(signature, "ShaderGen.create_decl_func(_,_)") == 0) return w_ShaderGen_create_decl_func;
    if (strcmp(signature, "ShaderGen.add_link_decl(_,_,_)") == 0) return w_ShaderGen_add_link_decl;
    
    if (strcmp(signature, "ShaderGen.create_func(_,_,_,_)") == 0) return w_ShaderGen_create_func;
    if (strcmp(signature, "ShaderGen.get_func_param(_,_)") == 0) return w_ShaderGen_get_func_param;
    if (strcmp(signature, "ShaderGen.get_func_args(_)") == 0) return w_ShaderGen_get_func_args;
    if (strcmp(signature, "ShaderGen.func_call(_,_,_)") == 0) return w_ShaderGen_func_call;

    if (strcmp(signature, "ShaderGen.if_block(_,_,_,_)") == 0) return w_shadergen_if_block;

    if (strcmp(signature, "ShaderGen.variable_float(_)") == 0) return w_ShaderGen_variable_float;
    if (strcmp(signature, "ShaderGen.variable_float2(_)") == 0) return w_ShaderGen_variable_float2;
    if (strcmp(signature, "ShaderGen.variable_float3(_)") == 0) return w_ShaderGen_variable_float3;
    if (strcmp(signature, "ShaderGen.variable_float4(_)") == 0) return w_ShaderGen_variable_float4;

    if (strcmp(signature, "ShaderGen.add_block(_,_)") == 0) return w_ShaderGen_add_block;

    if (strcmp(signature, "ShaderGen.const_bool(_,_)") == 0) return w_ShaderGen_const_bool;
    if (strcmp(signature, "ShaderGen.const_int(_,_)") == 0) return w_ShaderGen_const_int;
    
    if (strcmp(signature, "ShaderGen.const_float(_,_)") == 0) return w_ShaderGen_const_float;
    if (strcmp(signature, "ShaderGen.const_float2(_,_,_)") == 0) return w_ShaderGen_const_float2;
    if (strcmp(signature, "ShaderGen.const_float3(_,_,_,_)") == 0) return w_ShaderGen_const_float3;
    if (strcmp(signature, "ShaderGen.const_float4(_,_,_,_,_)") == 0) return w_ShaderGen_const_float4;

    if (strcmp(signature, "ShaderGen.const_mat2(_,_)") == 0) return w_ShaderGen_const_mat2;
    if (strcmp(signature, "ShaderGen.const_mat3(_,_)") == 0) return w_ShaderGen_const_mat3;
    if (strcmp(signature, "ShaderGen.const_mat4(_,_)") == 0) return w_ShaderGen_const_mat4;

    if (strcmp(signature, "ShaderGen.connect_cs_main(_)") == 0) return w_ShaderGen_connect_cs_main;

    // block

    if (strcmp(signature, "ShaderGen.is_equal(_,_,_)") == 0) return w_shadergen_is_equal;
    if (strcmp(signature, "ShaderGen.is_not_equal(_,_,_)") == 0) return w_shadergen_is_not_equal;
    if (strcmp(signature, "ShaderGen.is_greater(_,_,_)") == 0) return w_shadergen_is_greater;
    if (strcmp(signature, "ShaderGen.is_greater_equal(_,_,_)") == 0) return w_shadergen_is_greater_equal;
    if (strcmp(signature, "ShaderGen.is_less(_,_,_)") == 0) return w_shadergen_is_less;
    if (strcmp(signature, "ShaderGen.is_less_equal(_,_,_)") == 0) return w_shadergen_is_less_equal;

    if (strcmp(signature, "ShaderGen.kill(_)") == 0) return w_ShaderGen_kill;

    if (strcmp(signature, "ShaderGen.bb_return(_)") == 0) return w_ShaderGen_bb_return;
    if (strcmp(signature, "ShaderGen.bb_return_value(_,_)") == 0) return w_ShaderGen_bb_return_value;

    if (strcmp(signature, "ShaderGen.bb_store(_,_,_)") == 0) return w_ShaderGen_bb_store;

    return NULL;
}

void ShaderBindClass(const char* class_name, VesselForeignClassMethods* methods)
{
    if (strcmp(class_name, "ShaderGen") == 0)
    {
        methods->allocate = w_ShaderGen_allocate;
        methods->finalize = w_ShaderGen_finalize;
        return;
    }
}

}