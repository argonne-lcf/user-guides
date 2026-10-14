#!/bin/bash --

# Start serving pages
start_serving_pages()
{
  if [ -f .venv/bin/activate ];then
    source .venv/bin/activate && make serve
  else
    printf '%s\n' 'uv env not present on ./.venv - see Option 1 on user-guides README.md for installation instructions'
  fi
}

# Stop serving pages
stop_serving_pages()
{
  # Lets check whether there are any MkDocs processes listening on port range provided by find_port.py
  active_mkdocs="$(lsof -nPi4TCP:8000-8100 | grep -i 'listen' | awk '{print $2,$9}')"
  # If no matches, close and alert user that there were no running instances
  if [ -z "${active_mkdocs}" ];then
    echo 'No active MkDocs instances running...'
    exit
  fi
  # separate lsof output and collect start time for each pid
  declare -a mkdocs_collection
  while read -r line;do
    read -r mkdocs_pid mkdocs_port <<< "${line}"
    mkdocs_starttime="$(ps -p "${mkdocs_pid}" -o 'start' | tail -n +2)";
    mkdocs_collection+=("${mkdocs_pid} ${mkdocs_port} ${mkdocs_starttime}")
  done<<<"${active_mkdocs}"
  # Ask user to select/confirm MkDocs instance to close
  PS3='Select MkDocs instance to close: '
  echo '   PID   Port            Started'
  select session in "${mkdocs_collection[@]}";do
    read -r mkdocs_pid mkdocs_port mkdocs_starttime <<< "${session}"
    read -rp ">> ${mkdocs_port} started @ ${mkdocs_starttime} - Confirm (y or n) " RESPONSE
    if [[ "${RESPONSE}" =~ (y|Y) ]];then
      # Double check that pid is set and then kill pid
      test -n "${mkdocs_pid}" && kill -9 "${mkdocs_pid}"
      exit
    else
      echo "not closing..."
      exit
    fi
  done
}

# Explain usage of script
script_usage()
{
  cat <<-EOF
  Usage of $0

  Start a instance of MkDocs
  $0 start

  Stop serving an instance of MkDocs
  $0 stop

  Select MkDocs instance to close and confirm.
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
