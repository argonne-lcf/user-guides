#!/bin/bash --

# No frill start/stop serving pages

uv_pid="$(pgrep -xfu "${USER}" 'uv run --with-requirements requirements.txt make serve')"

# Start serving pages
start_serving_pages()
{
  # start pages or let the user know there are already some running
  if [ -n "${uv_pid}" ];then
    echo 'Pages already being served on http://127.0.0.1:8000 ...'
  else
    echo 'Serving pages on http://127.0.0.1:8000 ...'
    uv run --with-requirements requirements.txt make serve &>/dev/null &
  fi
}

# Stop serving pages
stop_serving_pages()
{
  # stop pages or let the user know there aren't any running
  if [ -n "${uv_pid}" ];then
    echo 'Stopping pages...'
    kill "${uv_pid}"
  else
    echo 'no running pages found...'
    echo "pgrep -u ${USER} uv"
    pgrep -u "${USER}" uv
  fi
}

# Explain usage of script
script_usage()
{
  cat <<-EOF
  Usage of $0

  Start serving pages
  $0 start

  Stop serving pages
  $0 stop
EOF
}

# if called with anyting but 1 arg, alert the user
if [ ${#@} -gt 1 ] || [ ${#@} -eq 0 ];then
  script_usage
  exit
fi

# mode select
case "$1" in
  'start')
    start_serving_pages
    ;;
    'stop')
    stop_serving_pages
    ;;
  *)
    script_usage
    ;;
esac
