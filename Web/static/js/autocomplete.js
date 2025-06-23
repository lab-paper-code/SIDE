let autocomplete = (function(){
    let _inp = null;
    let _arr = [];
    let _currentFocus;

    let _setAutocomplete = function(inp, arr){
        _arr = arr;
        if(_inp == inp)
            return;
        _removeListener();

        _inp = inp;
        _inp.addEventListener("input", inputEvent);
        _inp.addEventListener("keydown", keydownEvent);
    }

    let inputEvent = function(e){
        var a, b, i, val = this.value;

        closeAllLists();
        if(!val)
            return false;

        _currentFocus = -1;

        a = document.createElement("DIV");
        a.setAttribute("id", this.id + "autocomplete-list");
        a.setAttribute("class", "autocomplete-items");
        this.parentNode.appendChild(a);

        for(var i = 0; i< _arr.length; i++){
            if(_arr[i].substr(0, val.length) == val){
                b = document.createElement("DIV");
                b.innerHTML = "<strong>" + _arr[i].substr(0, val.length) + "</strong>";
                b.innerHTML += _arr[i].substr(val.length);
                b.innerHTML += "<input type='hidden' value='" + _arr[i] + "'>";

                b.addEventListener("click", function(e){
                    _inp.value = this.getElementsByTagName("input")[0].value;
                    closeAllLists();
                });
                a.appendChild(b);
            }
        }
    }
    let keydownEvent = function(e){
        var x = document.getElementById(this.id + "autocomplete-list");
        if (x){
            x = x.getElementsByTagName("div");
        }
        if (e.keyCode == 40){ // down
            _currentFocus++;
            addActive(x);
        }else if(e.keyCode == 38){ // up
            _currentFocus--;
            addActive(x);
        }else if(e.keyCode == 13){ // enter
            e.preventDefault();
            if(_currentFocus > -1){
                if(x) x[_currentFocus].click();
            }
        }
    }

    let addActive = function(x) {
        if (!x) return false;
        removeActive(x);
        if (_currentFocus >= x.length) _currentFocus = 0;
        if (_currentFocus < 0) _currentFocus = (x.length - 1);
        x[_currentFocus].classList.add("autocomplete-active");
    }
    let removeActive = function(x) {
        for (var i = 0; i < x.length; i++){
            x[i].classList.remove("autocomplete-active");
        }
    }
    let closeAllLists = function(element) {
        var x = document.getElementsByClassName("autocomplete-items");
        for (var i = 0; i < x.length; i++){
            if(element != x[i] && element != _inp)
                x[i].parentNode.removeChild(x[i]);
        }
    }
    let _removeListener = function() {
        if (_inp !== null) {
            console.log(_inp)
            _inp.removeEventListener("input", inputEvent, false);
            _inp.removeEventListener("keydown", keydownEvent, false);
        }
    }
    return {
        setAutocomplete: function(inp, arr){
            _setAutocomplete(inp, arr);
        }
    }
})();