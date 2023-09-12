discrete_deps = deps + [libdrm, libdrm_nouveau, libdrm_amdgpu]

if libdrm_amdgpu.found()
  amdgpu_discrete = executable('check-discrete-amdgpu',
    files('amdgpu.c'),
    dependencies: discrete_deps,
    install: true,
    install_dir: libexecdir,
  )
endif

if libdrm.found() and libdrm_nouveau.found()
  nouveau_discrete = executable('check-discrete-nouveau',
    files('nouveau.c'),
    dependencies: discrete_deps,
    install: true,
    install_dir: libexecdir,
  )
endif
