Shader hash a484b678-ebf65f44-d80c0563-f8c9d876

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
      dcl_temps 4
   0: ld_structured_indexable(structured_buffer, stride=48)(mixed,mixed,mixed,mixed) r0.xyz, l(0), l(0), pointLgihts.xyzx
   1: add r0.xyz, r0.xyzx, -v2.xyzx
   2: dp3 r0.w, r0.xyzx, r0.xyzx
   3: rsq r1.x, r0.w
   4: mul r0.xyz, r0.xyzx, r1.xxxx
   5: add r1.xyz, -v2.xyzx, cameraData.cameraPosition.xyzx
   6: dp3 r1.w, r1.xyzx, r1.xyzx
   7: rsq r1.w, r1.w
   8: mad r1.xyz, r1.xyzx, r1.wwww, r0.xyzx
   9: dp3 r1.w, r1.xyzx, r1.xyzx
  10: rsq r1.w, r1.w
  11: mul r1.xyz, r1.wwww, r1.xyzx
  12: dp3 r1.w, v1.xyzx, v1.xyzx
  13: rsq r1.w, r1.w
  14: mul r2.xyz, r1.wwww, v1.xyzx
  15: dp3_sat r1.x, r2.xyzx, r1.xyzx
  16: dp3 r0.x, r2.xyzx, r0.xyzx
  17: max r0.x, r0.x, l(0)
  18: mov r2.w, l(1.0000)
  19: ld_structured_indexable(structured_buffer, stride=48)(mixed,mixed,mixed,mixed) r3.xyzw, l(0), l(16), pointLgihts.xyzw
  20: mov r2.xyz, r3.xyzx
  21: div r2.xyzw, r2.xyzw, r0.wwww
  22: mul r3.xyzw, r3.wwww, r2.xyzw
  23: mad r0.xyzw, r3.xyzw, r0.xxxx, ambientLight.xxxx
  24: ld_structured_indexable(structured_buffer, stride=48)(mixed,mixed,mixed,mixed) r1.y, l(0), l(32), pointLgihts.xxxx
  25: mul r2.xyzw, r1.yyyy, r2.xyzw
  26: mad r0.xyzw, r2.xyzw, r1.xxxx, r0.xyzw
  27: sample_indexable(texture2d)(float,float,float,float) r1.xyzw, v3.xyxx, m_texture.xyzw, m_sampler
  28: mul r0.xyzw, r0.xyzw, r1.xyzw
  29: mad r1.xyz, r0.xyzx, l(2.5100, 2.5100, 2.5100, 0.0000), l(0.0300, 0.0300, 0.0300, 0.0000)
  30: mul r1.xyz, r0.xyzx, r1.xyzx
  31: mad r2.xyz, r0.xyzx, l(2.4300, 2.4300, 2.4300, 0.0000), l(0.5900, 0.5900, 0.5900, 0.0000)
  32: mad r0.xyz, r0.xyzx, r2.xyzx, l(0.1400, 0.1400, 0.1400, 0.0000)
  33: log r2.w, r0.w
  34: div_sat r0.xyz, r1.xyzx, r0.xyzx
  35: log r2.xyz, r0.xyzx
  36: mul r0.xyzw, r2.xyzw, l(0.4545, 0.4545, 0.4545, 0.4545)
  37: exp o0.xyzw, r0.xyzw
  38: ret