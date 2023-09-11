
if libdrm_amdgpu.found()
  executable('check-discrete-amdgpu',
    files('amdgpu.c'),
    dependencies: deps,
    install: true,
    install_dir: libexecdir,
  )
endif

if libdrm.found() and libdrm_nouveau.found()
  executable('check-discrete-nouveau',
    files('nouveau.c'),
    dependencies: deps,
    install: true,
    install_dir: libexecdir,
  )
endif
