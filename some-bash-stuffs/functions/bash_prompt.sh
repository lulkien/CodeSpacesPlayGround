bash_prompt() {
    local _USER=$(whoami)
    local _HOST=$(hostname)

    echo "[ \e[1;32m${_USER}\e[00m@\e[1;34m${_HOST} \e[1;36m\w\e[00m ]\n \$ "
}