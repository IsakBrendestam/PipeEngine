Shader hash 08cabf45-740d9dee-655f1179-2bfae777

ps_5_0
      dcl_globalFlags refactoringAllowed
      dcl_constantbuffer cb0[1] (CameraBuffer), immediateIndexed
      dcl_constantbuffer cb1[1] (LightData), immediateIndexed
      dcl_sampler m_sampler (s0), mode_default
      dcl_resource_texture2d (float,float,float,float) m_texture (t0)
      dcl_resource_structured pointLgihts (t5), 48
      dcl_input_ps linear v1.xyz
      dcl_input_ps linear v2.xyz
      dcl_input_ps linear v3.xy
      dcl_output o0.xyzw
      dcl_temps 10
   0: sample_indexable(texture2d)(float,float,float,float) r0.xyzw, v3.xyxx, m_texture.xyzw, m_sampler
   1: dp3 r1.x, v1.xyzx, v1.xyzx
   2: rsq r1.x, r1.x
   3: mul r1.xyz, r1.xxxx, v1.xyzx
   4: add r2.xyz, -v2.xyzx, cameraData.cameraPosition.xyzx
   5: dp3 r1.w, r2.xyzx, r2.xyzx
   6: rsq r1.w, r1.w
   7: mov r3.w, l(1.0000)
   8: mov r4.xyzw, l(0, 0, 0, 0)
   9: mov r5.xyzw, l(0, 0, 0, 0)
  10: mov r2.w, l(0)
  11: loop
  12:   ige r6.x, r2.w, pointLightCount.x
  13:   breakc_nz r6.x
  14:   ld_structured_indexable(structured_buffer, stride=48)(mixed,mixed,mixed,mixed) r6.xyz, r2.w, l(0), pointLgihts.xyzx
  15:   ld_structured_indexable(structured_buffer, stride=48)(mixed,mixed,mixed,mixed) r7.xyzw, r2.w, l(16), pointLgihts.xyzw
  16:   ld_structured_indexable(structured_buffer, stride=48)(mixed,mixed,mixed,mixed) r6.w, r2.w, l(32), pointLgihts.xxxx
  17:   add r6.xyz, r6.xyzx, -v2.xyzx
  18:   dp3 r8.x, r6.xyzx, r6.xyzx
  19:   rsq r8.y, r8.x
  20:   mul r6.xyz, r6.xyzx, r8.yyyy
  21:   dp3 r8.y, r1.xyzx, r6.xyzx
  22:   max r8.y, r8.y, l(0)
  23:   mov r3.xyz, r7.xyzx
  24:   div r9.xyzw, r3.xyzw, r8.xxxx
  25:   mul r7.xyzw, r7.wwww, r9.xyzw
  26:   mad r3.xyz, r2.xyzx, r1.wwww, r6.xyzx
  27:   dp3 r6.x, r3.xyzx, r3.xyzx
  28:   rsq r6.x, r6.x
  29:   mul r3.xyz, r3.xyzx, r6.xxxx
  30:   dp3_sat r3.x, r1.xyzx, r3.xyzx
  31:   mul r6.xyzw, r6.wwww, r9.xyzw
  32:   mad r4.xyzw, r7.xyzw, r8.yyyy, r4.xyzw
  33:   mad r5.xyzw, r6.xyzw, r3.xxxx, r5.xyzw
  34:   iadd r2.w, r2.w, l(1)
  35: endloop
  36: add r1.xyzw, r4.xyzw, ambientLight.xxxx
  37: add r1.xyzw, r5.xyzw, r1.xyzw
  38: mul r0.xyzw, r0.xyzw, r1.xyzw
  39: mad r1.xyz, r0.xyzx, l(2.5100, 2.5100, 2.5100, 0.0000), l(0.0300, 0.0300, 0.0300, 0.0000)
  40: mul r1.xyz, r0.xyzx, r1.xyzx
  41: mad r2.xyz, r0.xyzx, l(2.4300, 2.4300, 2.4300, 0.0000), l(0.5900, 0.5900, 0.5900, 0.0000)
  42: mad r0.xyz, r0.xyzx, r2.xyzx, l(0.1400, 0.1400, 0.1400, 0.0000)
  43: div_sat r0.xyz, r1.xyzx, r0.xyzx
  44: log r1.xyz, r0.xyzx
  45: log r1.w, r0.w
  46: mul r0.xyzw, r1.xyzw, l(0.4545, 0.4545, 0.4545, 0.4545)
  47: exp o0.xyzw, r0.xyzw
  48: ret