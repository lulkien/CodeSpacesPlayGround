cd() {
    declare -p BASH_CD_HISTORY      &>/dev/null || export BASH_CD_HISTORY=;
    declare -p BASH_CD_PREV         &>/dev/null || export BASH_CD_PREV=$HOME;
    declare -p BASH_CD_HISTORY_LEN  &>/dev/null || export BASH_CD_HISTORY_LEN=15;

    __cd_number_check() {
        [[ $@ =~ ^[0-9]+$ ]]
    }

    __cd_update_history() {
        local path=$1

        if [[ -z "$path" ]] || [[ "$path" = "$HOME" ]]; then
            return
        fi
                    
        if [[ -z "$BASH_CD_HISTORY" ]]; then
            BASH_CD_HISTORY=$path
        else
            BASH_CD_HISTORY=$path:$BASH_CD_HISTORY
        fi
    }

    __cd_remove_history() {
        local path=$1
        if [[ -z "$path" ]] || [[ "$path" = "$HOME" ]]; then
            return
        fi

        local history_paths=
        local history_len=0

        IFS=: read -r -a history_paths <<< "$BASH_CD_HISTORY"
        BASH_CD_HISTORY=

        for history in "${history_paths[@]}"; do

            if [[ $history = $path ]]; then
                continue
            fi

            history_len=$(( $history_len + 1 ))
            if [[ $history_len -ge $BASH_CD_HISTORY_LEN ]]; then
                break
            fi

            if [[ -z "$BASH_CD_HISTORY" ]]; then
                BASH_CD_HISTORY=$history
            else
                BASH_CD_HISTORY=$BASH_CD_HISTORY:$history
            fi
        done
    }

    __cd_print_history() {
        local index=1
        local history_paths=
        IFS=: read -r -a history_paths <<< "$BASH_CD_HISTORY"
        for path in "${history_paths[@]}"; do
            echo "$index - $path"
            index=$(( $index + 1 ))
        done
    }

    # ----------------------------------- MAIN -----------------------------------

    local cd_destination=
    local flag_index=
    local flag_list=false

    while [[ $# -gt 0 ]]; do
        case $1 in
            -l|--list)
                flag_list=true
                shift
                ;;
            -i|--index)
                flag_index="$2"
                if ! __cd_number_check $flag_index; then
                    echo "Index must be a number: $2"
                    return 1
                fi
                shift
                shift
                ;;
            -i*)
                flag_index=$(echo "$1" | sed 's/-i//')
                if ! __cd_number_check $flag_index; then
                    echo "Index must be a number: $flag_index"
                    return 1
                fi
                shift
                ;;
            --index=*)
                flag_index=$(echo "$1" | sed 's/--index=//')
                if ! __cd_number_check $flag_index; then
                    echo "Index must be a number: $flag_index"
                    return 1
                fi
                shift
                ;;
            --*)
                echo "Unknown argument: $1"
                return 2
                ;;
            -*) 
                if [[ "$1" = '-' ]]; then
                    flag_index=1
                    flag_prev=true
                else
                    flag_index=$(echo "$1" | sed 's/-//')
                    if ! __cd_number_check $flag_index; then
                        echo "Index must be a number: $flag_index"
                        return 1
                    fi
                fi
                shift
                ;;
            *)
                if [[ -z "$cd_destination" ]]; then
                    cd_destination="$1"
                    shift
                else
                    echo "Too many argument"
                    return 3
                fi
                ;;
        esac
    done

    if [[ -n "$flag_index" ]] && $flag_list; then
        echo "cd: Only use 1 option at a time"
        return 4
    fi

    if $flag_list; then
        if [[ -n "$cd_destination" ]]; then
            echo "cd: Print history only"
        fi
        __cd_print_history
        return 0
    fi

    if [[ -n "$flag_index" ]]; then
        local history_len=$(echo $BASH_CD_HISTORY | awk -F: '{print NF}')
        
        if [[ $flag_index -eq 1 ]] && [[ -z "$BASH_CD_HISTORY" ]]; then
            # CASE 1: empty history, try cd to prev -> goto $HOME
            cd_destination=$BASH_CD_PREV
        elif [[ $flag_index -lt 1 ]] || [[ $flag_index -gt $history_len ]]; then
            # CASE 2: index out of range
            echo "cd: Invalid index"
            return 5
        else
            # CASE 3: normal case
            if [[ -n "$cd_destination" ]]; then
                echo "cd: Change directory to index: $flag_index only"
            fi

            local history_paths=
            IFS=: read -r -a history_paths <<< "$BASH_CD_HISTORY"

            local index=$(( $flag_index - 1 ))
            cd_destination=${history_paths[$index]}
        fi
    fi

    if [[ -z "$cd_destination" ]]; then
        cd_destination=$HOME
    fi

    local cwd=$(pwd)
    if [[ "$cwd" = "$cd_destination" ]]; then
        return 0
    fi

    cd_destination=$(realpath -qs $cd_destination)
    if [[ -n "$flag_index" ]]; then
        echo "cd $cd_destination"
    fi

    builtin cd -- $cd_destination
    local cd_status=$?

    __cd_remove_history $cd_destination

    if [[ $cd_status -eq 0 ]]; then
        BASH_CD_PREV=$cwd
        __cd_update_history $cwd
    fi

    return $cd_status
}
