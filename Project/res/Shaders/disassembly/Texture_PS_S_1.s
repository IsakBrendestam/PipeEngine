Shader hash aab04344-63f1a3af-85091088-833bda43

ps_5_0
      dcl_globalFlags refactoringAllowed
      dcl_constantbuffer cb1[1] (LightData), immediateIndexed
      dcl_sampler m_sampler (s0), mode_default
      dcl_resource_texture2d (float,float,float,float) m_texture (t0)
      dcl_input_ps linear v3.xy
      dcl_output o0.xyzw
      dcl_temps 3
   0: sample_indexable(texture2d)(float,float,float,float) r0.xyzw, v3.xyxx, m_texture.xyzw, m_sampler
   1: mul r0.xyzw, r0.xyzw, ambientLight.xxxx
   2: mad r1.xyz, r0.xyzx, l(2.5100, 2.5100, 2.5100, 0.0000), l(0.0300, 0.0300, 0.0300, 0.0000)
   3: mul r1.xyz, r0.xyzx, r1.xyzx
   4: mad r2.xyz, r0.xyzx, l(2.4300, 2.4300, 2.4300, 0.0000), l(0.5900, 0.5900, 0.5900, 0.0000)
   5: mad r0.xyz, r0.xyzx, r2.xyzx, l(0.1400, 0.1400, 0.1400, 0.0000)
   6: log r2.w, r0.w
   7: div_sat r0.xyz, r1.xyzx, r0.xyzx
   8: log r2.xyz, r0.xyzx
   9: mul r0.xyzw, r2.xyzw, l(0.4545, 0.4545, 0.4545, 0.4545)
  10: exp o0.xyzw, r0.xyzw
  11: ret
