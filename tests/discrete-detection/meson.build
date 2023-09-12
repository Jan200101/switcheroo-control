
if libdrm_amdgpu.found()
    amdgpu_mock_lib = shared_library(
        'drm_amdgpu_mock',
        files('libdrm_amdgpu_mock.c'),
        dependencies: libdrm_amdgpu
    )

    test(
        'test amdgpu detection with invalid device',
        amdgpu_discrete,
        args: ['NO_GPU'],
        env: environment({'LD_PRELOAD': amdgpu_mock_lib.full_path()}),
        should_fail: true
    )

    test(
        'test amdgpu detection with non-AMD GPU',
        amdgpu_discrete,
        args: ['OTHER_GPU'],
        env: environment({'LD_PRELOAD': amdgpu_mock_lib.full_path()}),
        should_fail: true
    )

    test(
        'test amdgpu detection with AMD APU',
        amdgpu_discrete,
        args: ['AMD_APU'],
        env: environment({'LD_PRELOAD': amdgpu_mock_lib.full_path()}),
        should_fail: true
    )

    test(
        'test amdgpu detection with AMD GPU',
        amdgpu_discrete,
        args: ['AMD_GPU'],
        env: environment({'LD_PRELOAD': amdgpu_mock_lib.full_path()}),
        should_fail: false
    )
endif

if libdrm.found() and libdrm_nouveau.found()
    nouveau_mock_lib = shared_library(
        'drm_nouveau_mock',
        files('libdrm_nouveau_mock.c'),
        dependencies: [libdrm, libdrm_nouveau]
    )

    test(
        'test nouveau detection with invalid device',
        nouveau_discrete,
        args: ['NO_GPU'],
        env: environment({'LD_PRELOAD': nouveau_mock_lib.full_path()}),
        should_fail: true
    )

    test(
        'test nouveau detection with non-Nvidia GPU',
        nouveau_discrete,
        args: ['OTHER_GPU'],
        env: environment({'LD_PRELOAD': nouveau_mock_lib.full_path()}),
        should_fail: true
    )

    test(
        'test nouveau detection with Nvidia iGPU',
        nouveau_discrete,
        args: ['NVIDIA_IGPU'],
        env: environment({'LD_PRELOAD': nouveau_mock_lib.full_path()}),
        should_fail: true
    )

    test(
        'test nouveau detection with Nvidia GPU',
        nouveau_discrete,
        args: ['NVIDIA_GPU'],
        env: environment({'LD_PRELOAD': nouveau_mock_lib.full_path()}),
        should_fail: false
    )
endif