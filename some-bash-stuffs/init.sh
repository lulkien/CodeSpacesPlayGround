RUNTIME_DIR=$HOME/.config/bash

for config in $RUNTIME_DIR/conf.d/*.sh; do
    source $config
done

for func in $RUNTIME_DIR/functions/*.sh; do
    source $func
done

export PS1=$(bash_prompt)