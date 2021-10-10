# Make this library usable from the system's
# package manager.
headers_grabber = run_command('../scripts/grabber.sh')
if headers_grabber.returncode() == 0
  headers_sources = headers_grabber.stdout().strip().split('\n')
  install_headers(headers_sources, subdir : 'dibibase')
endif
