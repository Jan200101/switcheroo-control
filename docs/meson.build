content_files = files()

content_files += configure_file(
  input: 'version.xml.in',
  output: 'version.xml',
  configuration: version_conf,
)

content_files += gnome.gdbus_codegen(
  meson.project_name(),
  sources: meson.source_root() / 'src' / 'net.hadess.SwitcherooControl.xml',
  interface_prefix: 'net.hadess',
  namespace: 'SwitcherooControl',
  docbook: 'docs',
  build_by_default: true,
)

gnome.gtkdoc(
  meson.project_name(),
  main_xml: meson.project_name() + '-docs.xml',
  content_files: content_files,
  src_dir: [
    meson.source_root() /'src',
    meson.build_root() / 'src',
  ],
  ignore_headers: ['info-cleanup.h'],
  scan_args: ['--rebuild-sections'],
)
