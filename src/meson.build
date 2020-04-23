deps = [glib, gio, gudev]

sources = [
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

executable('switcheroo-control',
  sources, resources,
  dependencies: deps,
  install: true,
  install_dir: libexecdir,
)

configure_file(
  input: 'switcherooctl.in',
  output: 'switcherooctl',
  configuration: version_conf,
  install_dir: get_option('bindir')
)
