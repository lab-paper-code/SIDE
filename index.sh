config_path="Config/index.ini"
line="#################################################"

execute_trie() {
    echo
    echo "$line"
    echo "Do you want to execute the Index Server?"
    echo "1. YES"
    echo "2. NO"
    echo "$line"
    
    read -p "* Choose an option: " user_input
    echo

    if [ "$user_input" -eq 1 ]; then
        "src/TRIE" "../$config_path"
    elif [ "$user_input" -eq 2 ]; then
        echo "The binary file has been created at $trie_path."
    else
        echo "Invalid input. Exiting the program."
        exit 1
    fi
}


trie_path="Index/src"

# Check if the TRIE executable already exists
if [ -f "$trie_path/TRIE" ]; then
    echo "$line"
    echo "The TRIE executable already exists."
    echo "Do you want to execute it?"
    echo "1. YES"
    echo "2. NO"
    echo "$line"
    
    read -p "* Choose an option: " user_input
    echo

    if [ "$user_input" -eq 1 ]; then
        "$trie_path/TRIE" "$config_path"
    elif [ "$user_input" -eq 2 ]; then
        # Move to src and proceed with the build process
        cd "$trie_path/.." || exit 1
        make

        execute_trie
    else
        echo "Invalid input. Exiting the program."
    fi
else
    # If TRIE file doesn't exist, proceed with the build process
    cd "$trie_path/.." || exit 1
    make

    execute_trie
fi
