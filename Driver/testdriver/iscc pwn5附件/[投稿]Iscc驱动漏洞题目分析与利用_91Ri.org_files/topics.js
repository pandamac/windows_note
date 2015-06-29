(function(e) {
  e.isScrollToFixed = function(t) {
    return !!e(t).data("ScrollToFixed")
  }, e.ScrollToFixed = function(t, n) {
    function g() {
      s.trigger("preUnfixed.ScrollToFixed"), T(), s.trigger("unfixed.ScrollToFixed"), p = -1, l = s.offset().top, c = s.offset().left, r.options.offsets && (c += s.offset().left - s.position().left), h == -1 && (h = c), o = s.css("position"), i = !0, r.options.bottom != -1 && (s.trigger("preFixed.ScrollToFixed"), S(), s.trigger("fixed.ScrollToFixed"))
    }
    function y() {
      var e = r.options.limit;
      return e ? typeof e == "function" ? e.apply(s) : e : 0
    }
    function b() {
      return o === "fixed"
    }
    function w() {
      return o === "absolute"
    }
    function E() {
      return !b() && !w()
    }
    function S() {
      b() || (d.css({
        display: s.css("display"),
        width: s.outerWidth(!0),
        height: s.outerHeight(!0),
        "float": s.css("float")
      }), cssOptions = {
        position: "fixed",
        top: r.options.bottom == -1 ? C() : "",
        bottom: r.options.bottom == -1 ? "" : r.options.bottom,
        "margin-left": "0px"
      }, r.options.dontSetWidth || (cssOptions.width = s.width()), s.css(cssOptions), s.addClass("scroll-to-fixed-fixed"), r.options.className && s.addClass(r.options.className), o = "fixed")
    }
    function x() {
      var e = y(),
        t = c;
      r.options.removeOffsets && (t = 0, e -= l), cssOptions = {
        position: "absolute",
        top: e,
        left: t,
        "margin-left": "0px",
        bottom: ""
      }, r.options.dontSetWidth || (cssOptions.width = s.width()), s.css(cssOptions), o = "absolute"
    }
    function T() {
      E() || (p = -1, d.css("display", "none"), s.css({
        width: "",
        position: u,
        left: "",
        top: f.top,
        "margin-left": ""
      }), s.removeClass("scroll-to-fixed-fixed"), r.options.className && s.removeClass(r.options.className), o = null)
    }
    function N(e) {
      e != p && (s.css("left", c - e), p = e)
    }
    function C() {
      var e = r.options.marginTop;
      return e ? typeof e == "function" ? e.apply(s) : e : 0
    }
    function k() {
      if (!e.isScrollToFixed(s)) {
        return
      }
      var t = i;
      i || g();
      var n = e(window).scrollLeft(),
        o = e(window).scrollTop(),
        f = y();
      if (r.options.minWidth && e(window).width() < r.options.minWidth) {
        if (!E() || !t) {
          A(), s.trigger("preUnfixed.ScrollToFixed"), T(), s.trigger("unfixed.ScrollToFixed")
        }
      } else {
        if (r.options.bottom == -1) {
          if (f > 0 && o >= f - C()) {
            if (!w() || !t) {
              A(), s.trigger("preAbsolute.ScrollToFixed"), x(), s.trigger("unfixed.ScrollToFixed")
            }
          } else {
            if (o >= l - C()) {
              if (!b() || !t) {
                A(), s.trigger("preFixed.ScrollToFixed"), S(), p = -1, s.trigger("fixed.ScrollToFixed")
              }
              N(n)
            } else {
              if (!E() || !t) {
                A(), s.trigger("preUnfixed.ScrollToFixed"), T(), s.trigger("unfixed.ScrollToFixed")
              }
            }
          }
        } else {
          f > 0 ? o + e(window).height() - s.outerHeight(!0) >= f - (C() || -L()) ? b() && (A(), s.trigger("preUnfixed.ScrollToFixed"), u === "absolute" ? x() : T(), s.trigger("unfixed.ScrollToFixed")) : (b() || (A(), s.trigger("preFixed.ScrollToFixed"), S()), N(n), s.trigger("fixed.ScrollToFixed")) : N(n)
        }
      }
    }
    function L() {
      return r.options.bottom ? r.options.bottom : 0
    }
    function A() {
      var e = s.css("position");
      e == "absolute" ? s.trigger("postAbsolute.ScrollToFixed") : e == "fixed" ? s.trigger("postFixed.ScrollToFixed") : s.trigger("postUnfixed.ScrollToFixed")
    }
    var r = this;
    r.$el = e(t), r.el = t, r.$el.data("ScrollToFixed", r);
    var i = !1,
      s = r.$el,
      o, u, f, l = 0,
      c = 0,
      h = -1,
      p = -1,
      d = null,
      v, m, O = function(e) {
        s.is(":visible") && (i = !1, k())
      },
      M = function(e) {
        k()
      },
      _ = function() {
        var e = document.body;
        if (document.createElement && e && e.appendChild && e.removeChild) {
          var t = document.createElement("div");
          if (!t.getBoundingClientRect) {
            return null
          }
          t.innerHTML = "x", t.style.cssText = "position:fixed;top:100px;", e.appendChild(t);
          var n = e.style.height,
            r = e.scrollTop;
          e.style.height = "3000px", e.scrollTop = 500;
          var i = t.getBoundingClientRect().top;
          e.style.height = n;
          var s = i === 100;
          return e.removeChild(t), e.scrollTop = r, s
        }
        return null
      },
      D = function(e) {
        e = e || window.event, e.preventDefault && e.preventDefault(), e.returnValue = !1
      };
    r.init = function() {
      r.options = e.extend({}, e.ScrollToFixed.defaultOptions, n), r.$el.css("z-index", r.options.zIndex), d = e("<div />"), o = s.css("position"), u = s.css("position"), f = e.extend({}, s.offset()), E() && r.$el.after(d), e(window).bind("resize.ScrollToFixed", O), e(window).bind("scroll.ScrollToFixed", M), r.options.preFixed && s.bind("preFixed.ScrollToFixed", r.options.preFixed), r.options.postFixed && s.bind("postFixed.ScrollToFixed", r.options.postFixed), r.options.preUnfixed && s.bind("preUnfixed.ScrollToFixed", r.options.preUnfixed), r.options.postUnfixed && s.bind("postUnfixed.ScrollToFixed", r.options.postUnfixed), r.options.preAbsolute && s.bind("preAbsolute.ScrollToFixed", r.options.preAbsolute), r.options.postAbsolute && s.bind("postAbsolute.ScrollToFixed", r.options.postAbsolute), r.options.fixed && s.bind("fixed.ScrollToFixed", r.options.fixed), r.options.unfixed && s.bind("unfixed.ScrollToFixed", r.options.unfixed), r.options.spacerClass && d.addClass(r.options.spacerClass), s.bind("resize.ScrollToFixed", function() {
        d.height(s.height())
      }), s.bind("scroll.ScrollToFixed", function() {
        s.trigger("preUnfixed.ScrollToFixed"), T(), s.trigger("unfixed.ScrollToFixed"), k()
      }), s.bind("detach.ScrollToFixed", function(t) {
        D(t), s.trigger("preUnfixed.ScrollToFixed"), T(), s.trigger("unfixed.ScrollToFixed"), e(window).unbind("resize.ScrollToFixed", O), e(window).unbind("scroll.ScrollToFixed", M), s.unbind(".ScrollToFixed"), r.$el.removeData("ScrollToFixed")
      }), O()
    }, r.init()
  }, e.ScrollToFixed.defaultOptions = {
    marginTop: 0,
    limit: 0,
    bottom: -1,
    zIndex: 1000
  }, e.fn.scrollToFixed = function(t) {
    return this.each(function() {
      new e.ScrollToFixed(this, t)
    })
  }
})(jQuery), !
function(e) {
  e.matchMedia = e.matchMedia ||
  function(e) {
    var t, n = e.documentElement,
      r = n.firstElementChild || n.firstChild,
      i = e.createElement("body"),
      s = e.createElement("div");
    return s.id = "mq-test-1", s.style.cssText = "position:absolute;top:-100em", i.style.background = "none", i.appendChild(s), function(e) {
      return s.innerHTML = '&shy;<style media="' + e + '"> #mq-test-1 { width: 42px; }</style>', n.insertBefore(i, r), t = 42 === s.offsetWidth, n.removeChild(i), {
        matches: t,
        media: e
      }
    }
  }(e.document)
}(this), function(e) {
  function t() {
    w(!0)
  }
  var n = {};
  e.respond = n, n.update = function() {};
  var r = [],
    i = function() {
      var t = !1;
      try {
        t = new e.XMLHttpRequest
      } catch (n) {
        t = new e.ActiveXObject("Microsoft.XMLHTTP")
      }
      return function() {
        return t
      }
    }(),
    s = function(e, t) {
      var n = i();
      n && (n.open("GET", e, !0), n.onreadystatechange = function() {
        4 !== n.readyState || 200 !== n.status && 304 !== n.status || t(n.responseText)
      }, 4 !== n.readyState && n.send(null))
    };
  if (n.ajax = s, n.queue = r, n.regex = {
    media: /@media[^\{]+\{([^\{\}]*\{[^\}\{]*\})+/gi,
    keyframes: /@.*keyframes[^\{]+\{(?:[^\{\}]*\{[^\}\{]*\})+[^\}]+\}/gi,
    urls: /(url\()['"]?([^\/\)'"][^:\)'"]+)['"]?(\))/g,
    findStyles: /@media *([^\{]+)\{([\S\s]+?)$/,
    only: /(only\s+)?([a-zA-Z]+)\s?/,
    minw: /\([\s]*min\-width\s*:[\s]*([\s]*[0-9\.]+)(px|em)[\s]*\)/,
    maxw: /\([\s]*max\-width\s*:[\s]*([\s]*[0-9\.]+)(px|em)[\s]*\)/
  }, n.mediaQueriesSupported = e.matchMedia && null !== e.matchMedia("only all") && e.matchMedia("only all").matches, !n.mediaQueriesSupported) {
    var o, u, a, f = e.document,
      l = f.documentElement,
      c = [],
      h = [],
      p = [],
      d = {},
      v = 30,
      m = f.getElementsByTagName("head")[0] || l,
      g = f.getElementsByTagName("base")[0],
      y = m.getElementsByTagName("link"),
      b = function() {
        var e, t = f.createElement("div"),
          n = f.body,
          r = l.style.fontSize,
          i = n && n.style.fontSize,
          s = !1;
        return t.style.cssText = "position:absolute;font-size:1em;width:1em", n || (n = s = f.createElement("body"), n.style.background = "none"), l.style.fontSize = "100%", n.style.fontSize = "100%", n.appendChild(t), s && l.insertBefore(n, l.firstChild), e = t.offsetWidth, s ? l.removeChild(n) : n.removeChild(t), l.style.fontSize = r, i && (n.style.fontSize = i), e = a = parseFloat(e)
      },
      w = function(t) {
        var n = "clientWidth",
          r = l[n],
          i = "CSS1Compat" === f.compatMode && r || f.body[n] || r,
          s = {},
          d = y[y.length - 1],
          g = (new Date).getTime();
        if (t && o && v > g - o) {
          return e.clearTimeout(u), u = e.setTimeout(w, v), void 0
        }
        o = g;
        for (var E in c) {
          if (c.hasOwnProperty(E)) {
            var S = c[E],
              x = S.minw,
              T = S.maxw,
              N = null === x,
              C = null === T,
              L = "em";
            x && (x = parseFloat(x) * (x.indexOf(L) > -1 ? a || b() : 1)), T && (T = parseFloat(T) * (T.indexOf(L) > -1 ? a || b() : 1)), S.hasquery && (N && C || !(N || i >= x) || !(C || T >= i)) || (s[S.media] || (s[S.media] = []), s[S.media].push(h[S.rules]))
          }
        }
        for (var A in p) {
          p.hasOwnProperty(A) && p[A] && p[A].parentNode === m && m.removeChild(p[A])
        }
        for (var O in s) {
          if (s.hasOwnProperty(O)) {
            var M = f.createElement("style"),
              _ = s[O].join("\n");
            M.type = "text/css", M.media = O, m.insertBefore(M, d.nextSibling), M.styleSheet ? M.styleSheet.cssText = _ : M.appendChild(f.createTextNode(_)), p.push(M)
          }
        }
      },
      E = function(e, t, r) {
        var i = e.replace(n.regex.keyframes, "").match(n.regex.media),
          s = i && i.length || 0;
        t = t.substring(0, t.lastIndexOf("/"));
        var o = function(e) {
            return e.replace(n.regex.urls, "$1" + t + "$2$3")
          },
          u = !s && r;
        t.length && (t += "/"), u && (s = 1);
        for (var a = 0; s > a; a++) {
          var f, l, p, d;
          u ? (f = r, h.push(o(e))) : (f = i[a].match(n.regex.findStyles) && RegExp.$1, h.push(RegExp.$2 && o(RegExp.$2))), p = f.split(","), d = p.length;
          for (var v = 0; d > v; v++) {
            l = p[v], c.push({
              media: l.split("(")[0].match(n.regex.only) && RegExp.$2 || "all",
              rules: h.length - 1,
              hasquery: l.indexOf("(") > -1,
              minw: l.match(n.regex.minw) && parseFloat(RegExp.$1) + (RegExp.$2 || ""),
              maxw: l.match(n.regex.maxw) && parseFloat(RegExp.$1) + (RegExp.$2 || "")
            })
          }
        }
        w()
      },
      S = function() {
        if (r.length) {
          var t = r.shift();
          s(t.href, function(n) {
            E(n, t.href, t.media), d[t.href] = !0, e.setTimeout(function() {
              S()
            }, 0)
          })
        }
      },
      x = function() {
        for (var t = 0; t < y.length; t++) {
          var n = y[t],
            i = n.href,
            s = n.media,
            o = n.rel && "stylesheet" === n.rel.toLowerCase();
          i && o && !d[i] && (n.styleSheet && n.styleSheet.rawCssText ? (E(n.styleSheet.rawCssText, i, s), d[i] = !0) : (!/^([a-zA-Z:]*\/\/)/.test(i) && !g || i.replace(RegExp.$1, "").split("/")[0] === e.location.host) && ("//" === i.substring(0, 2) && (i = e.location.protocol + i), r.push({
            href: i,
            media: s
          })))
        }
        S()
      };
    x(), n.update = x, n.getEmValue = b, e.addEventListener ? e.addEventListener("resize", t, !1) : e.attachEvent && e.attachEvent("onresize", t)
  }
}(this), function(e) {
  var t, n, r, i, s, o, u, a = "Close",
    f = "BeforeClose",
    l = "AfterClose",
    c = "BeforeAppend",
    h = "MarkupParse",
    p = "Open",
    d = "Change",
    v = "mfp",
    m = "." + v,
    g = "mfp-ready",
    y = "mfp-removing",
    b = "mfp-prevent-close",
    w = function() {},
    E = !! window.jQuery,
    S = e(window),
    x = function(e, n) {
      t.ev.on(v + e + m, n)
    },
    T = function(t, n, r, i) {
      var s = document.createElement("div");
      return s.className = "mfp-" + t, r && (s.innerHTML = r), i ? n && n.appendChild(s) : (s = e(s), n && s.appendTo(n)), s
    },
    N = function(n, r) {
      t.ev.triggerHandler(v + n, r), t.st.callbacks && (n = n.charAt(0).toLowerCase() + n.slice(1), t.st.callbacks[n] && t.st.callbacks[n].apply(t, e.isArray(r) ? r : [r]))
    },
    C = function(n) {
      return n === u && t.currTemplate.closeBtn || (t.currTemplate.closeBtn = e(t.st.closeMarkup.replace("%title%", t.st.tClose)), u = n), t.currTemplate.closeBtn
    },
    k = function() {
      e.magnificPopup.instance || (t = new w, t.init(), e.magnificPopup.instance = t)
    },
    L = function() {
      var e = document.createElement("p").style,
        t = ["ms", "O", "Moz", "Webkit"];
      if (void 0 !== e.transition) {
        return !0
      }
      for (; t.length;) {
        if (t.pop() + "Transition" in e) {
          return !0
        }
      }
      return !1
    };
  w.prototype = {
    constructor: w,
    init: function() {
      var n = navigator.appVersion;
      t.isIE7 = -1 !== n.indexOf("MSIE 7."), t.isIE8 = -1 !== n.indexOf("MSIE 8."), t.isLowIE = t.isIE7 || t.isIE8, t.isAndroid = /android/gi.test(n), t.isIOS = /iphone|ipad|ipod/gi.test(n), t.supportsTransition = L(), t.probablyMobile = t.isAndroid || t.isIOS || /(Opera Mini)|Kindle|webOS|BlackBerry|(Opera Mobi)|(Windows Phone)|IEMobile/i.test(navigator.userAgent), r = e(document.body), i = e(document), t.popupsCache = {}
    },
    open: function(n) {
      var r;
      if (n.isObj === !1) {
        t.items = n.items.toArray(), t.index = 0;
        var s, u = n.items;
        for (r = 0; u.length > r; r++) {
          if (s = u[r], s.parsed && (s = s.el[0]), s === n.el[0]) {
            t.index = r;
            break
          }
        }
      } else {
        t.items = e.isArray(n.items) ? n.items : [n.items], t.index = n.index || 0
      }
      if (t.isOpen) {
        return t.updateItemHTML(), void 0
      }
      t.types = [], o = "", t.ev = n.mainEl && n.mainEl.length ? n.mainEl.eq(0) : i, n.key ? (t.popupsCache[n.key] || (t.popupsCache[n.key] = {}), t.currTemplate = t.popupsCache[n.key]) : t.currTemplate = {}, t.st = e.extend(!0, {}, e.magnificPopup.defaults, n), t.fixedContentPos = "auto" === t.st.fixedContentPos ? !t.probablyMobile : t.st.fixedContentPos, t.st.modal && (t.st.closeOnContentClick = !1, t.st.closeOnBgClick = !1, t.st.showCloseBtn = !1, t.st.enableEscapeKey = !1), t.bgOverlay || (t.bgOverlay = T("bg").on("click" + m, function() {
        t.close()
      }), t.wrap = T("wrap").attr("tabindex", -1).on("click" + m, function(e) {
        t._checkIfClose(e.target) && t.close()
      }), t.container = T("container", t.wrap)), t.contentContainer = T("content"), t.st.preloader && (t.preloader = T("preloader", t.container, t.st.tLoading));
      var a = e.magnificPopup.modules;
      for (r = 0; a.length > r; r++) {
        var f = a[r];
        f = f.charAt(0).toUpperCase() + f.slice(1), t["init" + f].call(t)
      }
      N("BeforeOpen"), t.st.showCloseBtn && (t.st.closeBtnInside ? (x(h, function(e, t, n, r) {
        n.close_replaceWith = C(r.type)
      }), o += " mfp-close-btn-in") : t.wrap.append(C())), t.st.alignTop && (o += " mfp-align-top"), t.fixedContentPos ? t.wrap.css({
        overflow: t.st.overflowY,
        overflowX: "hidden",
        overflowY: t.st.overflowY
      }) : t.wrap.css({
        top: S.scrollTop(),
        position: "absolute"
      }), (t.st.fixedBgPos === !1 || "auto" === t.st.fixedBgPos && !t.fixedContentPos) && t.bgOverlay.css({
        height: i.height(),
        position: "absolute"
      }), t.st.enableEscapeKey && i.on("keyup" + m, function(e) {
        27 === e.keyCode && t.close()
      }), S.on("resize" + m, function() {
        t.updateSize()
      }), t.st.closeOnContentClick || (o += " mfp-auto-cursor"), o && t.wrap.addClass(o);
      var l = t.wH = S.height(),
        c = {};
      if (t.fixedContentPos && t._hasScrollBar(l)) {
        var d = t._getScrollbarSize();
        d && (c.marginRight = d)
      }
      t.fixedContentPos && (t.isIE7 ? e("body, html").css("overflow", "hidden") : c.overflow = "hidden");
      var v = t.st.mainClass;
      return t.isIE7 && (v += " mfp-ie7"), v && t._addClassToMFP(v), t.updateItemHTML(), N("BuildControls"), e("html").css(c), t.bgOverlay.add(t.wrap).prependTo(document.body), t._lastFocusedEl = document.activeElement, setTimeout(function() {
        t.content ? (t._addClassToMFP(g), t._setFocus()) : t.bgOverlay.addClass(g), i.on("focusin" + m, t._onFocusIn)
      }, 16), t.isOpen = !0, t.updateSize(l), N(p), n
    },
    close: function() {
      t.isOpen && (N(f), t.isOpen = !1, t.st.removalDelay && !t.isLowIE && t.supportsTransition ? (t._addClassToMFP(y), setTimeout(function() {
        t._close()
      }, t.st.removalDelay)) : t._close())
    },
    _close: function() {
      N(a);
      var n = y + " " + g + " ";
      if (t.bgOverlay.detach(), t.wrap.detach(), t.container.empty(), t.st.mainClass && (n += t.st.mainClass + " "), t._removeClassFromMFP(n), t.fixedContentPos) {
        var r = {
          marginRight: ""
        };
        t.isIE7 ? e("body, html").css("overflow", "") : r.overflow = "", e("html").css(r)
      }
      i.off("keyup" + m + " focusin" + m), t.ev.off(m), t.wrap.attr("class", "mfp-wrap").removeAttr("style"), t.bgOverlay.attr("class", "mfp-bg"), t.container.attr("class", "mfp-container"), !t.st.showCloseBtn || t.st.closeBtnInside && t.currTemplate[t.currItem.type] !== !0 || t.currTemplate.closeBtn && t.currTemplate.closeBtn.detach(), t._lastFocusedEl && e(t._lastFocusedEl).focus(), t.currItem = null, t.content = null, t.currTemplate = null, t.prevHeight = 0, N(l)
    },
    updateSize: function(e) {
      if (t.isIOS) {
        var n = document.documentElement.clientWidth / window.innerWidth,
          r = window.innerHeight * n;
        t.wrap.css("height", r), t.wH = r
      } else {
        t.wH = e || S.height()
      }
      t.fixedContentPos || t.wrap.css("height", t.wH), N("Resize")
    },
    updateItemHTML: function() {
      var n = t.items[t.index];
      t.contentContainer.detach(), t.content && t.content.detach(), n.parsed || (n = t.parseEl(t.index));
      var r = n.type;
      if (N("BeforeChange", [t.currItem ? t.currItem.type : "", r]), t.currItem = n, !t.currTemplate[r]) {
        var i = t.st[r] ? t.st[r].markup : !1;
        N("FirstMarkupParse", i), t.currTemplate[r] = i ? e(i) : !0
      }
      s && s !== n.type && t.container.removeClass("mfp-" + s + "-holder");
      var o = t["get" + r.charAt(0).toUpperCase() + r.slice(1)](n, t.currTemplate[r]);
      t.appendContent(o, r), n.preloaded = !0, N(d, n), s = n.type, t.container.prepend(t.contentContainer), N("AfterChange")
    },
    appendContent: function(e, n) {
      t.content = e, e ? t.st.showCloseBtn && t.st.closeBtnInside && t.currTemplate[n] === !0 ? t.content.find(".mfp-close").length || t.content.append(C()) : t.content = e : t.content = "", N(c), t.container.addClass("mfp-" + n + "-holder"), t.contentContainer.append(t.content)
    },
    parseEl: function(n) {
      var r = t.items[n],
        i = r.type;
      if (r = r.tagName ? {
        el: e(r)
      } : {
        data: r,
        src: r.src
      }, r.el) {
        for (var s = t.types, o = 0; s.length > o; o++) {
          if (r.el.hasClass("mfp-" + s[o])) {
            i = s[o];
            break
          }
        }
        r.src = r.el.attr("data-mfp-src"), r.src || (r.src = r.el.attr("href"))
      }
      return r.type = i || t.st.type || "inline", r.index = n, r.parsed = !0, t.items[n] = r, N("ElementParse", r), t.items[n]
    },
    addGroup: function(e, n) {
      var r = function(r) {
          r.mfpEl = this, t._openClick(r, e, n)
        };
      n || (n = {});
      var i = "click.magnificPopup";
      n.mainEl = e, n.items ? (n.isObj = !0, e.off(i).on(i, r)) : (n.isObj = !1, n.delegate ? e.off(i).on(i, n.delegate, r) : (n.items = e, e.off(i).on(i, r)))
    },
    _openClick: function(n, r, i) {
      var s = void 0 !== i.midClick ? i.midClick : e.magnificPopup.defaults.midClick;
      if (s || 2 !== n.which && !n.ctrlKey && !n.metaKey) {
        var o = void 0 !== i.disableOn ? i.disableOn : e.magnificPopup.defaults.disableOn;
        if (o) {
          if (e.isFunction(o)) {
            if (!o.call(t)) {
              return !0
            }
          } else {
            if (o > S.width()) {
              return !0
            }
          }
        }
        n.type && (n.preventDefault(), t.isOpen && n.stopPropagation()), i.el = e(n.mfpEl), i.delegate && (i.items = r.find(i.delegate)), t.open(i)
      }
    },
    updateStatus: function(e, r) {
      if (t.preloader) {
        n !== e && t.container.removeClass("mfp-s-" + n), r || "loading" !== e || (r = t.st.tLoading);
        var i = {
          status: e,
          text: r
        };
        N("UpdateStatus", i), e = i.status, r = i.text, t.preloader.html(r), t.preloader.find("a").on("click", function(e) {
          e.stopImmediatePropagation()
        }), t.container.addClass("mfp-s-" + e), n = e
      }
    },
    _checkIfClose: function(n) {
      if (!e(n).hasClass(b)) {
        var r = t.st.closeOnContentClick,
          i = t.st.closeOnBgClick;
        if (r && i) {
          return !0
        }
        if (!t.content || e(n).hasClass("mfp-close") || t.preloader && n === t.preloader[0]) {
          return !0
        }
        if (n === t.content[0] || e.contains(t.content[0], n)) {
          if (r) {
            return !0
          }
        } else {
          if (i && e.contains(document, n)) {
            return !0
          }
        }
        return !1
      }
    },
    _addClassToMFP: function(e) {
      t.bgOverlay.addClass(e), t.wrap.addClass(e)
    },
    _removeClassFromMFP: function(e) {
      this.bgOverlay.removeClass(e), t.wrap.removeClass(e)
    },
    _hasScrollBar: function(e) {
      return (t.isIE7 ? i.height() : document.body.scrollHeight) > (e || S.height())
    },
    _setFocus: function() {
      (t.st.focus ? t.content.find(t.st.focus).eq(0) : t.wrap).focus()
    },
    _onFocusIn: function(n) {
      return n.target === t.wrap[0] || e.contains(t.wrap[0], n.target) ? void 0 : (t._setFocus(), !1)
    },
    _parseMarkup: function(t, n, r) {
      var i;
      r.data && (n = e.extend(r.data, n)), N(h, [t, n, r]), e.each(n, function(e, n) {
        if (void 0 === n || n === !1) {
          return !0
        }
        if (i = e.split("_"), i.length > 1) {
          var r = t.find(m + "-" + i[0]);
          if (r.length > 0) {
            var s = i[1];
            "replaceWith" === s ? r[0] !== n[0] && r.replaceWith(n) : "img" === s ? r.is("img") ? r.attr("src", n) : r.replaceWith('<img src="' + n + '" class="' + r.attr("class") + '" />') : r.attr(i[1], n)
          }
        } else {
          t.find(m + "-" + e).html(n)
        }
      })
    },
    _getScrollbarSize: function() {
      if (void 0 === t.scrollbarSize) {
        var e = document.createElement("div");
        e.id = "mfp-sbm", e.style.cssText = "width: 99px; height: 99px; overflow: scroll; position: absolute; top: -9999px;", document.body.appendChild(e), t.scrollbarSize = e.offsetWidth - e.clientWidth, document.body.removeChild(e)
      }
      return t.scrollbarSize
    }
  }, e.magnificPopup = {
    instance: null,
    proto: w.prototype,
    modules: [],
    open: function(t, n) {
      return k(), t = t ? e.extend(!0, {}, t) : {}, t.isObj = !0, t.index = n || 0, this.instance.open(t)
    },
    close: function() {
      return e.magnificPopup.instance && e.magnificPopup.instance.close()
    },
    registerModule: function(t, n) {
      n.options && (e.magnificPopup.defaults[t] = n.options), e.extend(this.proto, n.proto), this.modules.push(t)
    },
    defaults: {
      disableOn: 0,
      key: null,
      midClick: !1,
      mainClass: "",
      preloader: !0,
      focus: "",
      closeOnContentClick: !1,
      closeOnBgClick: !0,
      closeBtnInside: !0,
      showCloseBtn: !0,
      enableEscapeKey: !0,
      modal: !1,
      alignTop: !1,
      removalDelay: 0,
      fixedContentPos: "auto",
      fixedBgPos: "auto",
      overflowY: "auto",
      closeMarkup: '<button title="%title%" type="button" class="mfp-close">&times;</button>',
      tClose: "Close (Esc)",
      tLoading: "Loading..."
    }
  }, e.fn.magnificPopup = function(n) {
    k();
    var r = e(this);
    if ("string" == typeof n) {
      if ("open" === n) {
        var i, s = E ? r.data("magnificPopup") : r[0].magnificPopup,
          o = parseInt(arguments[1], 10) || 0;
        s.items ? i = s.items[o] : (i = r, s.delegate && (i = i.find(s.delegate)), i = i.eq(o)), t._openClick({
          mfpEl: i
        }, r, s)
      } else {
        t.isOpen && t[n].apply(t, Array.prototype.slice.call(arguments, 1))
      }
    } else {
      n = e.extend(!0, {}, n), E ? r.data("magnificPopup", n) : r[0].magnificPopup = n, t.addGroup(r, n)
    }
    return r
  };
  var A, O, M, _ = "inline",
    D = function() {
      M && (O.after(M.addClass(A)).detach(), M = null)
    };
  e.magnificPopup.registerModule(_, {
    options: {
      hiddenClass: "hide",
      markup: "",
      tNotFound: "Content not found"
    },
    proto: {
      initInline: function() {
        t.types.push(_), x(a + "." + _, function() {
          D()
        })
      },
      getInline: function(n, r) {
        if (D(), n.src) {
          var i = t.st.inline,
            s = e(n.src);
          if (s.length) {
            var o = s[0].parentNode;
            o && o.tagName && (O || (A = i.hiddenClass, O = T(A), A = "mfp-" + A), M = s.after(O).detach().removeClass(A)), t.updateStatus("ready")
          } else {
            t.updateStatus("error", i.tNotFound), s = e("<div>")
          }
          return n.inlineElement = s, s
        }
        return t.updateStatus("ready"), t._parseMarkup(r, {}, n), r
      }
    }
  });
  var P, H = "ajax",
    B = function() {
      P && r.removeClass(P)
    },
    j = function() {
      B(), t.req && t.req.abort()
    };
  e.magnificPopup.registerModule(H, {
    options: {
      settings: null,
      cursor: "mfp-ajax-cur",
      tError: '<a href="%url%">The content</a> could not be loaded.'
    },
    proto: {
      initAjax: function() {
        t.types.push(H), P = t.st.ajax.cursor, x(a + "." + H, j), x("BeforeChange." + H, j)
      },
      getAjax: function(n) {
        P && r.addClass(P), t.updateStatus("loading");
        var i = e.extend({
          url: n.src,
          success: function(r, i, s) {
            var o = {
              data: r,
              xhr: s
            };
            N("ParseAjax", o), t.appendContent(e(o.data), H), n.finished = !0, B(), t._setFocus(), setTimeout(function() {
              t.wrap.addClass(g)
            }, 16), t.updateStatus("ready"), N("AjaxContentAdded")
          },
          error: function() {
            B(), n.finished = n.loadError = !0, t.updateStatus("error", t.st.ajax.tError.replace("%url%", n.src))
          }
        }, t.st.ajax.settings);
        return t.req = e.ajax(i), ""
      }
    }
  });
  var F, I = function(n) {
      if (n.data && void 0 !== n.data.title) {
        return n.data.title
      }
      var r = t.st.image.titleSrc;
      if (r) {
        if (e.isFunction(r)) {
          return r.call(t, n)
        }
        if (n.el) {
          return n.el.attr(r) || ""
        }
      }
      return ""
    };
  e.magnificPopup.registerModule("image", {
    options: {
      markup: '<div class="mfp-figure"><div class="mfp-close"></div><figure><div class="mfp-img"></div><figcaption><div class="mfp-bottom-bar"><div class="mfp-title"></div><div class="mfp-counter"></div></div></figcaption></figure></div>',
      cursor: "mfp-zoom-out-cur",
      titleSrc: "title",
      verticalFit: !0,
      tError: '<a href="%url%">The image</a> could not be loaded.'
    },
    proto: {
      initImage: function() {
        var e = t.st.image,
          n = ".image";
        t.types.push("image"), x(p + n, function() {
          "image" === t.currItem.type && e.cursor && r.addClass(e.cursor)
        }), x(a + n, function() {
          e.cursor && r.removeClass(e.cursor), S.off("resize" + m)
        }), x("Resize" + n, t.resizeImage), t.isLowIE && x("AfterChange", t.resizeImage)
      },
      resizeImage: function() {
        var e = t.currItem;
        if (e && e.img && t.st.image.verticalFit) {
          var n = 0;
          t.isLowIE && (n = parseInt(e.img.css("padding-top"), 10) + parseInt(e.img.css("padding-bottom"), 10)), e.img.css("max-height", t.wH - n)
        }
      },
      _onImageHasSize: function(e) {
        e.img && (e.hasSize = !0, F && clearInterval(F), e.isCheckingImgSize = !1, N("ImageHasSize", e), e.imgHidden && (t.content && t.content.removeClass("mfp-loading"), e.imgHidden = !1))
      },
      findImageSize: function(e) {
        var n = 0,
          r = e.img[0],
          i = function(s) {
            F && clearInterval(F), F = setInterval(function() {
              return r.naturalWidth > 0 ? (t._onImageHasSize(e), void 0) : (n > 200 && clearInterval(F), n++, 3 === n ? i(10) : 40 === n ? i(50) : 100 === n && i(500), void 0)
            }, s)
          };
        i(1)
      },
      getImage: function(n, r) {
        var i = 0,
          s = function() {
            n && (n.img[0].complete ? (n.img.off(".mfploader"), n === t.currItem && (t._onImageHasSize(n), t.updateStatus("ready")), n.hasSize = !0, n.loaded = !0, N("ImageLoadComplete")) : (i++, 200 > i ? setTimeout(s, 100) : o()))
          },
          o = function() {
            n && (n.img.off(".mfploader"), n === t.currItem && (t._onImageHasSize(n), t.updateStatus("error", u.tError.replace("%url%", n.src))), n.hasSize = !0, n.loaded = !0, n.loadError = !0)
          },
          u = t.st.image,
          a = r.find(".mfp-img");
        if (a.length) {
          var f = document.createElement("img");
          f.className = "mfp-img", n.img = e(f).on("load.mfploader", s).on("error.mfploader", o), f.src = n.src, a.is("img") && (n.img = n.img.clone()), n.img[0].naturalWidth > 0 && (n.hasSize = !0)
        }
        return t._parseMarkup(r, {
          title: I(n),
          img_replaceWith: n.img
        }, n), t.resizeImage(), n.hasSize ? (F && clearInterval(F), n.loadError ? (r.addClass("mfp-loading"), t.updateStatus("error", u.tError.replace("%url%", n.src))) : (r.removeClass("mfp-loading"), t.updateStatus("ready")), r) : (t.updateStatus("loading"), n.loading = !0, n.hasSize || (n.imgHidden = !0, r.addClass("mfp-loading"), t.findImageSize(n)), r)
      }
    }
  });
  var q, R = function() {
      return void 0 === q && (q = void 0 !== document.createElement("p").style.MozTransform), q
    };
  e.magnificPopup.registerModule("zoom", {
    options: {
      enabled: !1,
      easing: "ease-in-out",
      duration: 300,
      opener: function(e) {
        return e.is("img") ? e : e.find("img")
      }
    },
    proto: {
      initZoom: function() {
        var e, n = t.st.zoom,
          r = ".zoom";
        if (n.enabled && t.supportsTransition) {
          var i, s, o = n.duration,
            u = function(e) {
              var t = e.clone().removeAttr("style").removeAttr("class").addClass("mfp-animated-image"),
                r = "all " + n.duration / 1000 + "s " + n.easing,
                i = {
                  position: "fixed",
                  zIndex: 9999,
                  left: 0,
                  top: 0,
                  "-webkit-backface-visibility": "hidden"
                },
                s = "transition";
              return i["-webkit-" + s] = i["-moz-" + s] = i["-o-" + s] = i[s] = r, t.css(i), t
            },
            l = function() {
              t.content.css("visibility", "visible")
            };
          x("BuildControls" + r, function() {
            if (t._allowZoom()) {
              if (clearTimeout(i), t.content.css("visibility", "hidden"), e = t._getItemToZoom(), !e) {
                return l(), void 0
              }
              s = u(e), s.css(t._getOffset()), t.wrap.append(s), i = setTimeout(function() {
                s.css(t._getOffset(!0)), i = setTimeout(function() {
                  l(), setTimeout(function() {
                    s.remove(), e = s = null, N("ZoomAnimationEnded")
                  }, 16)
                }, o)
              }, 16)
            }
          }), x(f + r, function() {
            if (t._allowZoom()) {
              if (clearTimeout(i), t.st.removalDelay = o, !e) {
                if (e = t._getItemToZoom(), !e) {
                  return
                }
                s = u(e)
              }
              s.css(t._getOffset(!0)), t.wrap.append(s), t.content.css("visibility", "hidden"), setTimeout(function() {
                s.css(t._getOffset())
              }, 16)
            }
          }), x(a + r, function() {
            t._allowZoom() && (l(), s && s.remove(), e = null)
          })
        }
      },
      _allowZoom: function() {
        return "image" === t.currItem.type
      },
      _getItemToZoom: function() {
        return t.currItem.hasSize ? t.currItem.img : !1
      },
      _getOffset: function(n) {
        var r;
        r = n ? t.currItem.img : t.st.zoom.opener(t.currItem.el || t.currItem);
        var i = r.offset(),
          s = parseInt(r.css("padding-top"), 10),
          o = parseInt(r.css("padding-bottom"), 10);
        i.top -= e(window).scrollTop() - s;
        var u = {
          width: r.width(),
          height: (E ? r.innerHeight() : r[0].offsetHeight) - o - s
        };
        return R() ? u["-moz-transform"] = u.transform = "translate(" + i.left + "px," + i.top + "px)" : (u.left = i.left, u.top = i.top), u
      }
    }
  });
  var U = "iframe",
    z = "//about:blank",
    W = function(e) {
      if (t.currTemplate[U]) {
        var n = t.currTemplate[U].find("iframe");
        n.length && (e || (n[0].src = z), t.isIE8 && n.css("display", e ? "block" : "none"))
      }
    };
  e.magnificPopup.registerModule(U, {
    options: {
      markup: '<div class="mfp-iframe-scaler"><div class="mfp-close"></div><iframe class="mfp-iframe" src="//about:blank" frameborder="0" allowfullscreen></iframe></div>',
      srcAction: "iframe_src",
      patterns: {
        youtube: {
          index: "youtube.com",
          id: "v=",
          src: "//www.youtube.com/embed/%id%?autoplay=1"
        },
        vimeo: {
          index: "vimeo.com/",
          id: "/",
          src: "//player.vimeo.com/video/%id%?autoplay=1"
        },
        gmaps: {
          index: "//maps.google.",
          src: "%id%&output=embed"
        }
      }
    },
    proto: {
      initIframe: function() {
        t.types.push(U), x("BeforeChange", function(e, t, n) {
          t !== n && (t === U ? W() : n === U && W(!0))
        }), x(a + "." + U, function() {
          W()
        })
      },
      getIframe: function(n, r) {
        var i = n.src,
          s = t.st.iframe;
        e.each(s.patterns, function() {
          return i.indexOf(this.index) > -1 ? (this.id && (i = "string" == typeof this.id ? i.substr(i.lastIndexOf(this.id) + this.id.length, i.length) : this.id.call(this, i)), i = this.src.replace("%id%", i), !1) : void 0
        });
        var o = {};
        return s.srcAction && (o[s.srcAction] = i), t._parseMarkup(r, o, n), t.updateStatus("ready"), r
      }
    }
  });
  var X = function(e) {
      var n = t.items.length;
      return e > n - 1 ? e - n : 0 > e ? n + e : e
    },
    V = function(e, t, n) {
      return e.replace(/%curr%/gi, t + 1).replace(/%total%/gi, n)
    };
  e.magnificPopup.registerModule("gallery", {
    options: {
      enabled: !1,
      arrowMarkup: '<button title="%title%" type="button" class="mfp-arrow mfp-arrow-%dir%"></button>',
      preload: [0, 2],
      navigateByImgClick: !0,
      arrows: !0,
      tPrev: "Previous (Left arrow key)",
      tNext: "Next (Right arrow key)",
      tCounter: "%curr% of %total%"
    },
    proto: {
      initGallery: function() {
        var n = t.st.gallery,
          r = ".mfp-gallery",
          s = Boolean(e.fn.mfpFastClick);
        return t.direction = !0, n && n.enabled ? (o += " mfp-gallery", x(p + r, function() {
          n.navigateByImgClick && t.wrap.on("click" + r, ".mfp-img", function() {
            return t.items.length > 1 ? (t.next(), !1) : void 0
          }), i.on("keydown" + r, function(e) {
            37 === e.keyCode ? t.prev() : 39 === e.keyCode && t.next()
          })
        }), x("UpdateStatus" + r, function(e, n) {
          n.text && (n.text = V(n.text, t.currItem.index, t.items.length))
        }), x(h + r, function(e, r, i, s) {
          var o = t.items.length;
          i.counter = o > 1 ? V(n.tCounter, s.index, o) : ""
        }), x("BuildControls" + r, function() {
          if (t.items.length > 1 && n.arrows && !t.arrowLeft) {
            var r = n.arrowMarkup,
              i = t.arrowLeft = e(r.replace(/%title%/gi, n.tPrev).replace(/%dir%/gi, "left")).addClass(b),
              o = t.arrowRight = e(r.replace(/%title%/gi, n.tNext).replace(/%dir%/gi, "right")).addClass(b),
              u = s ? "mfpFastClick" : "click";
            i[u](function() {
              t.prev()
            }), o[u](function() {
              t.next()
            }), t.isIE7 && (T("b", i[0], !1, !0), T("a", i[0], !1, !0), T("b", o[0], !1, !0), T("a", o[0], !1, !0)), t.container.append(i.add(o))
          }
        }), x(d + r, function() {
          t._preloadTimeout && clearTimeout(t._preloadTimeout), t._preloadTimeout = setTimeout(function() {
            t.preloadNearbyImages(), t._preloadTimeout = null
          }, 16)
        }), x(a + r, function() {
          i.off(r), t.wrap.off("click" + r), t.arrowLeft && s && t.arrowLeft.add(t.arrowRight).destroyMfpFastClick(), t.arrowRight = t.arrowLeft = null
        }), void 0) : !1
      },
      next: function() {
        t.direction = !0, t.index = X(t.index + 1), t.updateItemHTML()
      },
      prev: function() {
        t.direction = !1, t.index = X(t.index - 1), t.updateItemHTML()
      },
      goTo: function(e) {
        t.direction = e >= t.index, t.index = e, t.updateItemHTML()
      },
      preloadNearbyImages: function() {
        var e, n = t.st.gallery.preload,
          r = Math.min(n[0], t.items.length),
          i = Math.min(n[1], t.items.length);
        for (e = 1;
        (t.direction ? i : r) >= e; e++) {
          t._preloadItem(t.index + e)
        }
        for (e = 1;
        (t.direction ? r : i) >= e; e++) {
          t._preloadItem(t.index - e)
        }
      },
      _preloadItem: function(n) {
        if (n = X(n), !t.items[n].preloaded) {
          var r = t.items[n];
          r.parsed || (r = t.parseEl(n)), N("LazyLoad", r), "image" === r.type && (r.img = e('<img class="mfp-img" />').on("load.mfploader", function() {
            r.hasSize = !0
          }).on("error.mfploader", function() {
            r.hasSize = !0, r.loadError = !0, N("LazyLoadError", r)
          }).attr("src", r.src)), r.preloaded = !0
        }
      }
    }
  });
  var $ = "retina";
  e.magnificPopup.registerModule($, {
    options: {
      replaceSrc: function(e) {
        return e.src.replace(/\.\w+$/, function(e) {
          return "@2x" + e
        })
      },
      ratio: 1
    },
    proto: {
      initRetina: function() {
        if (window.devicePixelRatio > 1) {
          var e = t.st.retina,
            n = e.ratio;
          n = isNaN(n) ? n() : n, n > 1 && (x("ImageHasSize." + $, function(e, t) {
            t.img.css({
              "max-width": t.img[0].naturalWidth / n,
              width: "100%"
            })
          }), x("ElementParse." + $, function(t, r) {
            r.src = e.replaceSrc(r, n)
          }))
        }
      }
    }
  }), function() {
    var t = 1000,
      n = "ontouchstart" in window,
      r = function() {
        S.off("touchmove" + s + " touchend" + s)
      },
      i = "mfpFastClick",
      s = "." + i;
    e.fn.mfpFastClick = function(i) {
      return e(this).each(function() {
        var o, u = e(this);
        if (n) {
          var a, f, l, c, h, p;
          u.on("touchstart" + s, function(e) {
            c = !1, p = 1, h = e.originalEvent ? e.originalEvent.touches[0] : e.touches[0], f = h.clientX, l = h.clientY, S.on("touchmove" + s, function(e) {
              h = e.originalEvent ? e.originalEvent.touches : e.touches, p = h.length, h = h[0], (Math.abs(h.clientX - f) > 10 || Math.abs(h.clientY - l) > 10) && (c = !0, r())
            }).on("touchend" + s, function(e) {
              r(), c || p > 1 || (o = !0, e.preventDefault(), clearTimeout(a), a = setTimeout(function() {
                o = !1
              }, t), i())
            })
          })
        }
        u.on("click" + s, function() {
          o || i()
        })
      })
    }, e.fn.destroyMfpFastClick = function() {
      e(this).off("touchstart" + s + " click" + s), n && S.off("touchmove" + s + " touchend" + s)
    }
  }(), k()
}(window.jQuery || window.Zepto), !
function(e, t, n, r) {
  function i() {
    h = y.scrollTop(), p = h + (t.innerHeight || y.height()), d = t.pageXOffset || 0, v = d + (t.innerWidth || y.width())
  }
  function s(t, n) {
    n.trigger("lazy" + t, [n]);
    var r = m["on" + t];
    r && (e.isFunction(r) ? r.call(n[0]) : n.addClass(r.addClass).removeClass(r.removeClass)), f()
  }
  function o(t) {
    s(t.type, e(this).off("load error", o))
  }
  function u(t) {
    if (w.length) {
      S = 1 / 0, i();
      for (var r = w.length - 1; r >= 0; r--) {
        var u = w[r],
          a = u[0],
          f = u.lazyLoadXT;
        if (e.contains(n.documentElement, a)) {
          if (t || !f.visibleOnly || a.offsetWidth > 0 || a.offsetHeight > 0) {
            var l = u.offset(),
              c = l.top,
              m = l.left,
              g = f.edgeX,
              y = f.edgeY,
              b = c - y;
            if (t || p >= b && c + u.height() > h - y && v + g >= m && m + u.width() > d - g) {
              s("show", u);
              var E = f.srcAttr,
                x = e.isFunction(E) ? E(u) : u.attr(E);
              x && u.on("load error", o).attr("src", x), w.splice(r, 1)
            } else {
              S > b && (S = b)
            }
          }
        } else {
          w.splice(r, 1)
        }
      }
      w.length || e(n).trigger("lazyloadall")
    }
  }
  function a() {
    x > 1 ? (x = 1, u(), setTimeout(a, m.throttle)) : x = 0
  }
  function f(e) {
    w.length && (e && "scroll" === e.type && (i(), S >= p) || (x || setTimeout(a, 0), x = 2))
  }
  function l() {
    y.lazyLoadXT(), f()
  }
  function c() {
    u(!0)
  }
  var h, p, d, v, m = {
    autoInit: !0,
    selector: "img",
    classNojs: "lazy",
    blankImage: "data:image/gif;base64,R0lGODlhAQABAIAAAAAAAP///yH5BAEAAAAALAAAAAABAAEAAAIBRAA7",
    throttle: 99,
    loadEvent: "pageshow",
    updateEvent: "load orientationchange resize scroll",
    forceEvent: "",
    oninit: null,
    onshow: null,
    onload: null,
    onerror: null
  },
    g = {
      srcAttr: "data-src",
      edgeX: 0,
      edgeY: 0,
      visibleOnly: !0
    },
    y = e(t),
    b = "lazied",
    w = [],
    E = e.data ||
  function(t, n) {
    return e(t).data(n)
  }, S = 0, x = 0;
  e.lazyLoadXT = e.extend(m, g, e.lazyLoadXT), e.fn.lazyLoadXT = function(n) {
    n = n || {};
    var i, o = {},
      u = n.blankImage || m.blankImage,
      a = n.classNojs || m.classNojs,
      f = n.checkDuplicates || !0;
    for (i in g) {
      o[i] = n[i] === r ? m[i] : n[i]
    }
    return this.each(function() {
      if (this === t) {
        e(m.selector).lazyLoadXT(n)
      } else {
        if (f && E(this, b)) {
          return
        }
        var r = e(this);
        r.data(b, 1).removeClass(a), u && "IMG" === r[0].tagName && !r.attr("src") && r.attr("src", u), r.lazyLoadXT = e.extend({}, o), s("init", r), w.unshift(r)
      }
    })
  }, e(n).ready(function() {
    y.on(m.loadEvent, l).on(m.updateEvent, f).on(m.forceEvent, c), m.autoInit && l()
  })
}(window.jQuery || window.Zepto, window, document), function(e, t) {
  "object" == typeof exports ? t(exports) : "function" == typeof define && define.amd ? define(["exports"], t) : t(e)
}(this, function(e) {
  function t(e) {
    this._targetElement = e, this._options = {
      wideAreaAttr: "data-widearea",
      exitOnEsc: !0,
      defaultColorScheme: "light",
      closeIconLabel: "退出全屏",
      changeThemeIconLabel: "关灯",
      fullScreenIconLabel: "WideArea Mode"
    };
    var t = this;
    e = this._targetElement.querySelectorAll("textarea[" + this._options.wideAreaAttr + "='enable']");
    for (var i = function() {
        r.call(t, this)
      }, s = e.length - 1; 0 <= s; s--) {
      var o = e[s],
        u = document.createElement("div"),
        a = document.createElement("div"),
        f = document.createElement("a");
      u.className = "widearea-wrapper", a.className = "widearea-icons", f.className = "widearea-icon fullscreen", f.title = this._options.fullScreenIconLabel, u.style.width = parseInt(n(o, "width")) + "px", u.style.height = n(o, "height"), o.style.width = n(o, "width"), o.style.height = n(o, "height"), f.href = "javascript:void(0);", f.onclick = i;
      var l = o.cloneNode();
      l.value = o.value, u.appendChild(l), a.appendChild(f), u.appendChild(a), o.parentNode.replaceChild(u, o)
    }
  }
  function n(e, t) {
    var n = "";
    return e.currentStyle ? n = e.currentStyle[t] : document.defaultView && document.defaultView.getComputedStyle && (n = document.defaultView.getComputedStyle(e, null).getPropertyValue(t)), n.toLowerCase ? n.toLowerCase() : n
  }
  function r(e) {
    var t = this;
    e = e.parentNode.parentNode.querySelector("textarea");
    var n = e.cloneNode();
    n.className = ("widearea-fullscreen " + e.className).replace(/^\s+|\s+$/g, ""), e.className = ("widearea-fullscreened " + e.className).replace(/^\s+|\s+$/g, "");
    var r = document.createElement("div");
    r.className = "widearea-controlPanel";
    var o = document.createElement("a");
    o.href = "javascript:void(0);", o.className = "widearea-icon close", o.title = this._options.closeIconLabel, o.onclick = function() {
      s.call(t)
    };
    var u = document.createElement("a");
    u.href = "javascript:void(0);", u.className = "widearea-icon changeTheme", u.title = this._options.changeThemeIconLabel, u.onclick = function() {
      i.call(t)
    }, r.appendChild(o), r.appendChild(u), o = document.createElement("div"), o.className = "widearea-overlayLayer " + this._options.defaultColorScheme, o.appendChild(n), o.appendChild(r), document.body.appendChild(o), n.focus(), n.value = e.value, this._onKeyDown = function(e) {
      27 === e.keyCode && t._options.exitOnEsc && s.call(t), 9 == e.keyCode && (e.preventDefault(), e = n.selectionStart, n.value = n.value.substring(0, e) + "  " + n.value.substring(n.selectionEnd), n.selectionEnd = e + 1)
    }, window.addEventListener ? window.addEventListener("keydown", t._onKeyDown, !0) : document.attachEvent && document.attachEvent("onkeydown", t._onKeyDown)
  }
  function i() {
    var e = document.querySelector(".widearea-overlayLayer");
    e.className = /dark/gi.test(e.className) ? e.className.replace("dark", "light") : e.className.replace("light", "dark")
  }
  function s() {
    var e = document.querySelector("textarea.widearea-fullscreened"),
      t = document.querySelector(".widearea-overlayLayer"),
      n = t.querySelector("textarea");
    e.focus(), e.value = n.value, e.className = e.className.replace(/widearea-fullscreened/gi, ""), t.parentNode.removeChild(t), window.removeEventListener ? window.removeEventListener("keydown", this._onKeyDown, !0) : document.detachEvent && document.detachEvent("onkeydown", this._onKeyDown)
  }
  var o = function(e) {
      if ("string" == typeof e) {
        if (e = document.querySelector(e)) {
          return new t(e)
        }
        throw Error("There is no element with given selector.")
      }
      return new t(document.body)
    };
  return o.version = "0.1.3", o.fn = t.prototype = {
    clone: function() {
      return new t(this)
    },
    setOption: function(e, t) {
      return this._options[e] = t, this
    },
    setOptions: function(e) {
      var t = this._options,
        n = {},
        r;
      for (r in t) {
        n[r] = t[r]
      }
      for (r in e) {
        n[r] = e[r]
      }
      return this._options = n, this
    }
  }, e.wideArea = o
}), function(e) {
  e(function() {
    e(".tabs a, .tabs button").click(function(t) {
      t.preventDefault();
      var n = e(this),
        r = n.attr("data-target");
      n.addClass("active").parent().addClass("active"), n.siblings().removeClass("active"), n.parent("li").siblings().removeClass("active").children().removeClass("active"), e(r).addClass("active"), e(r).siblings().removeClass("active")
    })
  })
}(jQuery), function(e) {
  e(function() {
    e("body").addClass("js")
  })
}(jQuery), function(e) {
  function n() {
    if (t.width() < 934) {
      return e(".aside-social").removeClass("aside-fixed")
    }
    e(".aside-social").addClass("aside-fixed")
  }
  function r() {
    e(".drop, .overlay, .single-post-social__popup").removeClass("active"), setTimeout(function() {
      e(".login-section, .register-section").removeClass("fadeOut"), e(".login-section").removeClass("fadeInDown"), e(".register-section").removeClass("fadeInUp")
    }, 300)
  }
  var t = e(window);
  t.resize(n).trigger("resize"), e(document).on("scroll", function() {
    e(this).scrollTop() > 40 ? e(".header-normal").addClass("scrolled") : e(".header-normal").removeClass("scrolled")
  }), window.matchMedia("(min-width: 768px)").matches && e(".drop").on("click", function(t) {
    t.stopPropagation(), t.preventDefault(), e(".drop.active").not(this).removeClass("active"), e(this).toggleClass("active")
  }), e(".toggle__main").on("click", function(t) {
    t.stopPropagation(), e(".main-nav").toggleClass("active").siblings().removeClass("active")
  }), e(".toggle__search").on("click", function(t) {
    t.stopPropagation(), e(".search").toggleClass("active").siblings().removeClass("active")
  }), e(".share-btn").on("click", function(t) {
    t.stopPropagation(), e(".single-post-social__popup").toggleClass("active").siblings().removeClass("active")
  }), setTimeout(function () {
    e(document).scrollTop(0); e(".side-fixed").scrollToFixed({
    marginTop: e(".header").outerHeight() + 10,
    dontCheckForPositionFixedSupport: !0,
    limit: e(".footer").offset().top - e(".side-fixed").outerHeight() - 10,
    dontSetWidth: true,
    preAbsolute: function () {
    },
    unfixed: function () {
      var me = e(this);
      if (me.css("position") == 'relative') {
        me.css({
          top: 0,
          left: 0
        });
      }
    }
  })
  }, 1000), e(".dropdown__list, .panel").on("click", function(e) {
    e.stopPropagation()
  }), e(document).on("click", function() {
    r()
  }), e(document).on("keyup", function() {
    event.keyCode === 27 && r()
  }), e(document).on("scroll", function() {
    e(".drop").removeClass("active")
  }), e("a[href*='http://']:not([href*='" + location.hostname + "']), [href*='https://']:not([href*='" + location.hostname + "'])").addClass("external").attr("target", "_blank"), e(".overlay").magnificPopup({
    mainClass: "pop-in",
    removalDelay: 400,
    preloader: !1,
    showCloseBtn: !1,
    midClick: !0
  }), e(".login__register-btn").on("click", function() {
    e(".login-section").removeClass("fadeInDown").addClass("fadeOut"), setTimeout(function() {
      e(".register-section").removeClass("fadeOut").addClass("fadeInUp")
    }, 200)
  }), e(".register__login-btn").on("click", function() {
    e(".register-section").removeClass("fadeInUp").addClass("fadeOut"), setTimeout(function() {
      e(".login-section").removeClass("fadeOut").addClass("fadeInDown")
    }, 200)
  }), e(".icon-heart, .icon-star").on("click", function() {
    e(this).toggleClass("active")
  }), e(".comment-submit-btn").on("click", function() {
    e("#btn_reply").trigger("click")
  }), e(".dropdown_login_out_link").on("click", function() {
    e("#login_out_link").trigger("click")
  }), e("a.long-weibo").on("click", function() {
    w_img = e(".longweibo-image"), w_img.attr("src", w_img.attr("data-src"))
  });

}(jQuery), function() {
  window.Topics = {
    appendImageFromUpload: function(e) {
      var t, n, r, i, s, o, u, a, f;
      u = $(".topic_editor"), r = $("#topic_feature_img"), n = u.caretPos(), o = "";
      for (a = 0, f = e.length; a < f; a++) {
        s = e[a], o = "![](" + s + ")\n"
      }
      return i = u.val(), r.val().length === 58 && r.val(s), t = i.slice(0, n), u.val(t + o + i.slice(n + 1, i.count)), u.caretPos(n + o.length), u.focus()
    },
    initUploader: function() {
      var e;
      $("#topic_add_image").bind("click", function() {
        return $("#topic_upload_images").click(), !1
      }), e = {
        url: "/photos",
        type: "POST",
        beforeSend: function() {
          return $("#topic_add_image").hide(), $("#topic_add_image").before("<span class='loading'>上传中...</span>")
        },
        success: function(e, t, n) {
          return $("#topic_add_image").parent().find("span").remove(), $("#topic_add_image").show(), Topics.appendImageFromUpload([e])
        }
      };
      if ($("#topic_upload_images").length !== 0) {
        return $("#topic_upload_images").fileUpload(e), !1
      }
    },
    reply: function(e, t) {
      var n, r;
      return r = $("#reply_body"), n = "#" + e + "楼 @" + t + " ", r.val().trim().length === 0 ? n += "" : n = "\n" + n, r.focus().val(r.val() + n), !1
    },
    hightlightReply: function(e) {
      return $("#replies .reply").removeClass("light"), $("#reply" + e).addClass("light")
    },
    checkRepliesLikeStatus: function(e) {
      var t, n, r, i, s;
      s = [];
      for (r = 0, i = e.length; r < i; r++) {
        n = e[r], t = $("#replies a.likeable[data-id=" + n + "]"), s.push(App.likeableAsLiked(t))
      }
      return s
    },
    replyCallback: function(e, t) {
      return $("#main .alert-message").remove(), e ? ($("abbr.timeago", $("#replies .comment").last()).timeago(), $("#new_reply textarea").val(""), $("#preview").text(""), App.notice(t, "#reply")) : App.alert(t, "#reply"), $("#new_reply textarea").focus(), $(".comment-submit-btn").delay(800).html("发表评论")
    },
    preview: function(e) {
      return $("#preview").text("Loading..."), $.post("/p/preview", {
        body: e
      }, function(e) {
        return $("#preview").html(e.body)
      }, "json")
    },
    hookPreview: function(e, t) {
      var n;
      return n = $(document.createElement("div")).attr("id", "preview"), n.addClass("entry-content body"), $(t).after(n), n.hide(), $(".edit_btn", e).click(function() {
        return $(".preview", e).removeClass("active"), $(this).parent().addClass("active"), $(n).hide(), $(t).show(), !1
      }), $(".preview_btn", e).click(function() {
        return $(".edit", e).removeClass("active"), $(this).parent().addClass("active"), $(n).show(), $(t).hide(), Topics.preview($(t).val()), !1
      })
    },
    initCloseWarning: function(e, t) {
      return e.length === 0 ? !1 : (t || (t = "正文内容已更改, 提交前建议复制正文，防止以外丢失!"), $("input[type=submit]").click(function() {
        return $(window).unbind("beforeunload")
      }), e.change(function() {
        return e.val().length > 0 ? $(window).bind("beforeunload", function(e) {
          return $.browser.msie ? e.returnValue = t : t
        }) : $(window).unbind("beforeunload")
      }))
    },
    favorite: function(e) {
      var t, n;
      return n = $(e).data("id"), $(".star-btn i").hasClass("active") ? ($.post("/p/" + n + "/favorite"), $(e).attr("title", "取消收藏")) : (t = {
        type: "unfavorite"
      }, $.ajax({
        url: "/p/" + n + "/favorite",
        data: t,
        type: "POST"
      }), $(e).attr("title", "收藏")), !1
    },
    follow: function(e) {
      var t, n;
      return n = $(e).data("id"), t = $(e).data("followed"), t ? ($.ajax({
        url: "/p/" + n + "/unfollow",
        type: "POST"
      }), $(e).data("followed", !1), $(e).text("+ 关注"), $(e).attr("title", "关注后续评论"), $("i", e).attr("class", "icon icon-eye-close")) : ($.ajax({
        url: "/p/" + n + "/follow",
        type: "POST"
      }), $(e).data("followed", !0), $(e).text("关注中"), $(e).attr("title", "再次点击取消关注"), $("i", e).attr("class", "btn btn-success btn-mini")), !1
    }
  }, $(document).ready(function() {
    var e, t, n, r;
    return Topics.initCloseWarning($("textarea.closewarning")), $("#new_reply").submit(function() {
      return $(".comment-submit-btn").html("Loading")
    }), Topics.initUploader(), $("a.at_floor").on("click", function() {
      return Topics.hightlightReply($(this).data("floor"))
    }), $("a.reply, a.at-reply").on("click", function() {
      return Topics.reply($(this).data("floor"), $(this).data("login"))
    }), Topics.hookPreview($(".editor_toolbar"), $(".topic_editor")), r = [], n = [], (e = $("#topic_show .leader a[data-author]")).length > 0 && (t = {
      login: e.text(),
      name: e.data("name")
    }, r.push(t), n.push(t.login)), $("#replies span.name a").each(function(e) {
      var t;
      t = {
        login: $(this).text(),
        name: $(this).data("name")
      };
      if ($.inArray(t.login, n) < 0) {
        return n.push(t.login), r.push(t)
      }
    }), App.atReplyable("textarea", r), $("body.topics-controller.new-action #topic_title").focus()
  })
}.call(this);