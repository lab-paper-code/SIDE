(function($) {
    var start = function(el,options) {
        var magnifier = null;
        var bigImageDOM = null;
        var leftDistance = undefined;
        var topDistance = undefined;
        var config = {
            width: 400,
            position: 'right',
            background: '#fff',
            opacity: 0.7, 
            distance: 20 
        };

        var notTouchBorder = function() {
            if (
                leftDistance >= config.width / 4 &&
                leftDistance <= 3 * config.width / 4 &&
                topDistance >= config.width / 4 &&
                topDistance <= 3 * config.width / 4
            ) {
                return true;
            }
            return false;
        };
        var prepareDOM = function(dom) {
            dom.each(function() {
                var img = $(this)
                    .find('img')
                    .clone();
                $(this).append(
                    $(
                        '<div class="small"><div class="magnifier"></div></div></div><div class="big"></div>'
                    )
                );
                $(this)
                    .children('div')
                    .append(img);
                $(this)
                    .find('img')
                    .first()
                    .remove();
            });
        };

        var bindListener = function(dom) {
            dom.each(function() {
                $(this)
                    .find('.small')
                    .mouseenter(function(e) {
                        enterLogic($(this), e);
                    })
                    .mousemove(function(e) {
                        var offset = $(this).offset();
                        var scrollTop =
                            document.body.scrollTop ||
                            document.documentElement.scrollTop;
                        var scrollLeft =
                            document.body.scrollLeft ||
                            document.documentElement.scrollLeft;

                        leftDistance = e.clientX - (offset.left - scrollLeft);
                        topDistance = e.clientY - (offset.top - scrollTop);

                        mainLogic();
                    })
                    .mouseleave(function(e) {
                        $(this)
                            .siblings('.big')
                            .hide();
                        magnifier.hide();
                        resetVar();
                    });
            });
        };

        var enterLogic = function($this, event) {
            var offset = $this.offset();
            var scrollTop =
                document.body.scrollTop || document.documentElement.scrollTop;
            var scrollLeft =
                document.body.scrollLeft || document.documentElement.scrollLeft;

            leftDistance = event.clientX - (offset.left - scrollLeft);
            topDistance = event.clientY - (offset.top - scrollLeft);

            magnifier = $this.find('.magnifier');
            var big = $this.siblings('.big');
            bigImageDOM = big.find('img');
            if (
                leftDistance <= config.width / 4 &&
                topDistance <= config.width / 4
            ) {
                magnifier.css({
                    top: 0,
                    left: 0
                });
                bigImageDOM.css({
                    top: 0,
                    left: 0
                });
                
            } else if (
                leftDistance <= config.width / 4 &&
                topDistance >= 3 * config.width / 4
            ) {
                magnifier.css({
                    top: config.width / 2 + 'px',
                    left: 0
                });
                bigImageDOM.css({
                    top: '-' + config.width + 'px',
                    left: 0
                });
                
            } else if (
                leftDistance >= 3 * config.width / 4 &&
                topDistance <= config.width / 4
            ) {
                magnifier.css({
                    top: 0,
                    left: config.width / 2 + 'px'
                });
                bigImageDOM.css({
                    top: 0,
                    left: '-' + config.width + 'px'
                });
                
            } else if (
                leftDistance >= 3 * config.width / 4 &&
                topDistance >= 3 * config.width / 4
            ) {
                magnifier.css({
                    top: config.width / 2 + 'px',
                    left: config.width / 2 + 'px'
                });
                bigImageDOM.css({
                    top: '-' + config.width + 'px',
                    left: '-' + config.width + 'px'
                });
            }
            magnifier.show();
            big.show();
        };

        
        var mainLogic = function() {
        
            if (notTouchBorder(leftDistance, topDistance)) {
                magnifier.css({
                    left: leftDistance - config.width / 4 + 'px',
                    top: topDistance - config.width / 4 + 'px'
                });
                bigImageDOM.css({
                    left: -2 * (leftDistance - config.width / 4) + 'px',
                    top: -2 * (topDistance - config.width / 4) + 'px'
                });
            } else {
                if (
                    leftDistance - config.width / 4 >= 0 &&
                    leftDistance - 3 * config.width / 4 <= 0
                ) {
                    magnifier.css({
                        left: leftDistance - config.width / 4 + 'px'
                    });
                    bigImageDOM.css({
                        left: -2 * (leftDistance - config.width / 4) + 'px'
                    });
                } else if (leftDistance < config.width / 4) {
                    magnifier.css({
                        left: '0px'
                    });
                    bigImageDOM.css({
                        left: '0px'
                    });
                } else if (leftDistance > 3 * config.width / 4) {
                    magnifier.css({
                        left: config.width / 2 + 'px'
                    });
                    bigImageDOM.css({
                        left: '-' + config.width + 'px'
                    });
                }
                
                if (
                    topDistance - config.width / 4 >= 0 &&
                    topDistance - 3 * config.width / 4 <= 0
                ) {
                    magnifier.css({
                        top: topDistance - config.width / 4 + 'px'
                    });
                    bigImageDOM.css({
                        top: -2 * (topDistance - config.width / 4) + 'px'
                    });
                } else if (topDistance < config.width / 4) {
                    magnifier.css({
                        top: '0px'
                    });
                    bigImageDOM.css({
                        top: '0px'
                    });
                } else if (topDistance > 3 * config.width / 4) {
                    magnifier.css({
                        top: config.width / 2 + 'px'
                    });
                    bigImageDOM.css({
                        top: '-' + config.width + 'px'
                    });
                }
            }
        };

        
        var setStyle = function(dom) {
            dom.css({
                width: config.width + 'px',
                height: config.width + 'px'
            });
            dom.find('.small,.big,.small img').css({
                width: config.width + 'px',
                height: config.width + 'px'
            });
            dom.find('.big img').css({
                width: 2 * config.width + 'px',
                height: 2 * config.width + 'px'
            });
            dom.find('.magnifier').css({
                width: config.width / 2 + 'px',
                height: config.width / 2 + 'px',
                opacity: config.opacity,
                background: config.background
            });

        
            if (config.position === 'left') {
                dom.find('.big').css({
                    left: '-' + (config.width + config.distance) + 'px'
                });
            } else if (config.position === 'right') {
                dom.find('.big').css({
                    right: '-' + (config.width + config.distance) + 'px'
                });
            } else if (config.position === 'top') {
                dom.find('.big').css({
                    top: '-' + (config.width + config.distance) + 'px'
                });
            } else if (config.position === 'bottom') {
                dom.find('.big').css({
                    bottom: '-' + (config.width + config.distance) + 'px'
                });
            }
        };

        
        var resetVar = function() {
            magnifier = null;
            bigImageDOM = null;
            leftDistance = undefined;
            topDistance = undefined;
        };

        
        config = $.extend({}, config, options);
        el.each(function() {
            var $this = $(this);
            resetVar();
            prepareDOM($this);
            bindListener($this);
            setStyle($this);
        });
    };

    $.fn.hiZoom = function(options) {
        start($(this), options);
    };
})(jQuery);
