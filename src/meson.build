deps = [glib, gio, gudev, libdrm, libdrm_nouveau, libdrm_amdgpu]

sources = [
  'discrete-detection.c',
  'discrete-detection.h',
  'info-cleanup.c',
  'info-cleanup.h',
  'switcheroo-control.c',
]

resources = gnome.compile_resources(
  'switcheroo-control-resources',
  'switcheroo-control.gresource.xml',
  source_dir: '.',
  c_name: 'switcheroo_control',
  export: true,
)

switcheroo_cargs = []
if libdrm.found() and libdrm_nouveau.found()
  switcheroo_cargs += '-DHAS_LIBDRM_NOUVEAU'
endif
if libdrm_amdgpu.found()
  switcheroo_cargs += '-DHAS_LIBDRM_AMDGPU'
endif


executable('switcheroo-control',
  sources, resources,
  dependencies: deps,
  c_args: switcheroo_cargs,
  install: true,
  install_dir: libexecdir,
)

python = import('python')
py_installation = python.find_installation('python3', required: true)

switcherooctl_conf = configuration_data()
switcherooctl_conf.set('VERSION', meson.project_version())
switcherooctl_conf.set('PYTHON3', py_installation.path())

configure_file(
  input: 'switcherooctl.in',
  output: 'switcherooctl',
  configuration: switcherooctl_conf,
  install_dir: get_option('bindir')
)
