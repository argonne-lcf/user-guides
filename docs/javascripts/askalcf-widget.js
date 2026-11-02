/* AskALCF floating chat widget.
 *
 * A floating chat bubble that talks to the AskALCF conversational agent
 * (RAG over the ALCF docs + public HPC tools): tool-call display and
 * multi-turn conversation. Anonymous: no sign-in, and no credentials are sent.
 *
 * The API base URL comes from window.ASKALCF_API_BASE, set in
 * overrides/main.html; without it, the widget isn't built.
 *
 * Built once and appended to <body>, so it survives Material's instant
 * navigation (the body element persists; only the content region is swapped).
 */
(function () {
  "use strict";
  if (window.__askalcfWidgetInit || !window.ASKALCF_API_BASE) return;
  window.__askalcfWidgetInit = true;

  var API_BASE = window.ASKALCF_API_BASE.replace(/\/+$/, "");
  var ENDPOINT = API_BASE + "/v1/agent/chat";
  // Resolve against this script's URL, like the CSS's ../images/ paths, so the
  // logo loads under any base path (PR previews, a site_url with a subpath).
  // document.currentScript is only set while the script first runs.
  var LOGO = window.ASKALCF_LOGO_URL ||
    new URL("../images/askalcf-welcome.svg", document.currentScript.src).href;
  var STORE_KEY = "askalcf:v1";

  var CHAT_ICON =
    '<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" ' +
    'stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">' +
    '<path d="M21 11.5a8.5 8.5 0 0 1-12.3 7.6L3 21l1.9-5.7A8.5 8.5 0 1 1 21 11.5z"/>' +
    '<circle cx="8.5" cy="12" r="1.1" fill="currentColor" stroke="none"/>' +
    '<circle cx="12" cy="12" r="1.1" fill="currentColor" stroke="none"/>' +
    '<circle cx="15.5" cy="12" r="1.1" fill="currentColor" stroke="none"/>' +
    "</svg>";
  // Header button icons, drawn on one grid so they match in size and
  // alignment (text glyphs such as ↻ ✕ ⤢ vary with the font).
  function headerIcon(d) {
    return '<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" ' +
      'stroke-linecap="round" stroke-linejoin="round" aria-hidden="true"><path d="' + d + '"/></svg>';
  }
  var MAX_ICON = headerIcon("M15 3h6v6M9 21H3v-6M21 3l-7 7M3 21l7-7");
  var RESTORE_ICON = headerIcon("M4 14h6v6M20 10h-6V4M14 10l7-7M3 21l7-7");
  var CLEAR_ICON = headerIcon("M21 12a9 9 0 1 1-9-9c2.52 0 4.93 1 6.74 2.74L21 8M21 3v5h-5");
  var CLOSE_ICON = headerIcon("M19 5L5 19M5 5l14 14");
  // The launcher's robot, inline so CSS can paint its face (currentColor) per
  // color scheme. The eyes and mouth are cut out, so the button color shows.
  var LAUNCHER_ICON =
    '<svg viewBox="108 126 1036 1036" aria-hidden="true" focusable="false">' +
    '<mask id="askalcf-face-cutouts"><path fill="#fff" d="M522 282H730A210 210 0 0 1 940 ' +
    '492V689A95 95 0 0 1 845 784H407A95 95 0 0 1 312 689V492A210 210 0 0 1 522 282Z"/>' +
    '<g fill="#000"><circle cx="476" cy="553" r="68"/><circle cx="776" cy="553" r="68"/>' +
    '<rect x="528" y="666" width="198" height="56" rx="28"/></g></mask>' +
    '<rect x="312" y="282" width="628" height="502" fill="currentColor" mask="url(#askalcf-face-cutouts)"/>' +
    '<g fill="currentColor"><rect x="220" y="454" width="68" height="200" rx="34"/>' +
    '<rect x="964" y="454" width="68" height="200" rx="34"/>' +
    '<path d="M266 936C266 924 266 814 626 814C986 814 986 924 986 936C986 1070 806 1058 ' +
    '626 1058C446 1058 266 1070 266 936Z"/></g></svg>';

  var state = { messages: [], conversationId: null, open: false, max: false };
  var busy = false;
  // Bumped by "New chat", so a reply to a discarded conversation is dropped.
  var generation = 0;
  var els = {};

  // ---- persistence -------------------------------------------------------
  function load() {
    try {
      var raw = sessionStorage.getItem(STORE_KEY);
      if (raw) {
        var d = JSON.parse(raw);
        state.messages = Array.isArray(d.messages) ? d.messages : [];
        state.conversationId = d.conversationId || null;
        state.open = !!d.open;
        state.max = !!d.max;
      }
    } catch (e) { /* ignore */ }
  }
  function save() {
    try { sessionStorage.setItem(STORE_KEY, JSON.stringify(state)); } catch (e) { /* ignore */ }
  }

  // ---- escaping & markdown ----------------------------------------------
  function escapeHtml(s) {
    return String(s == null ? "" : s)
      .replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;")
      .replace(/"/g, "&quot;").replace(/'/g, "&#39;");
  }
  // Raw HTML in an answer is shown as text, not parsed: a placeholder such as
  // <project> stays visible, and an answer can't add markup to the page.
  var mdRenderer;
  function renderMarkdown(text) {
    if (window.marked && window.DOMPurify) {
      try {
        if (!mdRenderer) {
          mdRenderer = new window.marked.Renderer();
          // marked 12 passes a string; later versions pass a token.
          mdRenderer.html = function (h) { return escapeHtml(typeof h === "string" ? h : h.text); };
        }
        var html = window.marked.parse(String(text || ""), { breaks: true, renderer: mdRenderer });
        // A second layer, since answers come from a model: drop markup that
        // could restyle or cover the page, or pose as a form (DOMPurify keeps
        // all of these by default).
        return window.DOMPurify.sanitize(html, {
          ADD_ATTR: ["target", "rel"],
          FORBID_TAGS: ["style", "form", "input", "button", "textarea", "select", "option"],
          FORBID_ATTR: ["style"]
        });
      } catch (e) { /* fall through */ }
    }
    return escapeHtml(text).replace(/\n/g, "<br>");
  }

  // ---- tool calls --------------------------------------------------------
  function renderToolCalls(toolCalls) {
    if (!toolCalls || !toolCalls.length) return "";
    return toolCalls.map(function (tc) {
      var args = Object.keys(tc.args || {}).map(function (k) {
        return k + "=" + JSON.stringify(tc.args[k]);
      }).join(", ");
      return '<details class="askalcf-tool"><summary>🔧 ' +
        escapeHtml(tc.name) + "(" + escapeHtml(args) + ")</summary><pre>" +
        escapeHtml(tc.result) + "</pre></details>";
    }).join("");
  }

  // ---- rendering ---------------------------------------------------------
  // Append-only: rows already on screen stay, so an open tool-call block stays
  // open, focus stays put, and only new bubbles animate. New chat empties
  // state.messages, which redraws from the welcome screen.
  function render() {
    var box = els.messages;
    var thinking = box.querySelector(".askalcf-thinking");
    if (thinking) box.removeChild(thinking.closest(".askalcf-msg"));
    var shown = box.querySelectorAll(".askalcf-msg").length;
    if (!state.messages.length || shown > state.messages.length) {
      box.innerHTML = "";
      shown = 0;
    }

    if (!state.messages.length) {
      var w = document.createElement("div");
      w.className = "askalcf-welcome";
      w.innerHTML = '<img src="' + LOGO + '" alt="AskALCF">' +
        "<strong>Welcome to AskALCF</strong>Ask about ALCF systems,<br>" +
        "software, jobs, and documentation.<br>Ask me anything to get started.";
      box.appendChild(w);
    } else {
      var welcome = box.querySelector(".askalcf-welcome");
      if (welcome) box.removeChild(welcome);
    }

    state.messages.slice(shown).forEach(function (m) {
      var row = document.createElement("div");
      row.className = "askalcf-msg " + (m.role === "user" ? "user" : "assistant");
      var bubble = document.createElement("div");
      bubble.className = "askalcf-bubble";
      // Tell screen readers who wrote each bubble.
      var who = '<span class="askalcf-sr-only">' + (m.role === "user" ? "You said:" : "AskALCF said:") + " </span>";
      if (m.role === "user") {
        bubble.innerHTML = who + escapeHtml(m.content);
      } else if (m.error) {
        bubble.innerHTML = who + '<span class="askalcf-error">Sorry, something went wrong: ' +
          escapeHtml(m.error) + "</span>";
      } else {
        bubble.innerHTML = who + renderToolCalls(m.tool_calls) + renderMarkdown(m.content);
      }
      row.appendChild(bubble);
      box.appendChild(row);
    });

    if (busy) {
      var t = document.createElement("div");
      t.className = "askalcf-msg assistant";
      t.innerHTML = '<div class="askalcf-bubble"><div class="askalcf-thinking">' +
        "<span></span><span></span><span></span></div></div>";
      box.appendChild(t);
    }
    scrollBottom();
  }
  function scrollBottom() {
    requestAnimationFrame(function () { els.messages.scrollTop = els.messages.scrollHeight; });
  }
  function setBusy(v) {
    busy = v;
    els.send.disabled = v;
    // readOnly, not disabled: disabling the focused textarea drops focus.
    els.input.readOnly = v;
  }
  // Screen readers aren't told about render()'s rebuilt transcript, so
  // announce each reply in a separate live region.
  var announceTimer;
  function announce(text) {
    clearTimeout(announceTimer);
    els.live.textContent = "";
    announceTimer = setTimeout(function () {
      els.live.textContent = text;
      // Clear it again, so the old status isn't found later while browsing.
      announceTimer = setTimeout(function () { els.live.textContent = ""; }, 10000);
    }, 50);
  }

  // ---- networking --------------------------------------------------------
  function send() {
    var text = els.input.value.trim();
    if (!text || busy) return;
    els.input.value = "";
    autoSize();
    state.messages.push({ role: "user", content: text });
    setBusy(true);
    // Disabling a focused Send button drops focus; keep it in the textarea.
    els.input.focus();
    render();
    save();

    var payload = {
      messages: state.messages.map(function (m) {
        return { role: m.role, content: m.content };
      }),
      conversation_id: state.conversationId || null
    };

    var gen = generation;
    fetch(ENDPOINT, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload)
    }).then(function (res) {
      if (res.ok) return res.json();
      // Show the API's reason (rate or size limit, outage) when it gives one.
      return res.json().catch(function () { return null; }).then(function (d) {
        var detail = d && d.detail;
        var why = typeof detail === "string" ? detail : (detail && detail.message) || "";
        // The API's 500 message asks users to quote its error_code, so show it.
        var tail = [detail && detail.error_code && "error code " + detail.error_code,
          "HTTP " + res.status].filter(Boolean).join(", ");
        throw new Error(why ? why + " (" + tail + ")" : tail);
      });
    }).then(function (data) {
      if (gen !== generation) return;
      state.conversationId = data.conversation_id || state.conversationId;
      state.messages.push({
        role: "assistant",
        content: data.content || "",
        tool_calls: data.tool_calls || []
      });
    }).catch(function (err) {
      if (gen !== generation) return;
      state.messages.push({ role: "assistant", content: "", error: String(err.message || err) });
    }).then(function () {
      if (gen !== generation) return;
      var last = state.messages[state.messages.length - 1];
      setBusy(false);
      render();
      save();
      announce(last.error ? "AskALCF error: " + last.error : "AskALCF replied.");
    });
  }

  // ---- input sizing ------------------------------------------------------
  function autoSize() {
    var ta = els.input;
    ta.style.height = "auto";
    ta.style.height = Math.min(ta.scrollHeight, 120) + "px";
  }

  // ---- open/close --------------------------------------------------------
  function setMax(v) {
    state.max = v;
    els.root.classList.toggle("askalcf-max", v);
    els.max.setAttribute("aria-pressed", String(v));
    els.max.title = v ? "Restore size" : "Maximize";
    els.max.innerHTML = v ? RESTORE_ICON : MAX_ICON;
    save();
  }
  // focus=false when the page load restores an open panel: moving focus then
  // would take it from the page the reader just opened.
  function setOpen(v, focus) {
    state.open = v;
    els.root.classList.toggle("askalcf-open", v);
    save();
    if (focus === false) return;
    if (v) setTimeout(function () { els.input.focus(); }, 50);
    else els.fab.focus();  // back to the control that opened the panel
  }

  // ---- launcher placement ------------------------------------------------
  // When the table-of-contents column is visible, line the launcher up with
  // the column's text, next to the page being read. Otherwise (narrow
  // screens, pages without a TOC) the CSS corner position applies.
  function placeLauncher() {
    var title = document.querySelector(".md-sidebar--secondary .md-nav__title");
    var r = title && title.getBoundingClientRect();
    var visible = !!r && r.width > 0 && r.right <= window.innerWidth;
    els.fab.style.left = visible ?
      Math.round(r.left + parseFloat(getComputedStyle(title).paddingLeft)) + "px" : "";
    els.fab.style.right = visible ? "auto" : "";
    // The CSS pads the end of the TOC by the launcher's height, so its last
    // entries can scroll out from under the button.
    document.documentElement.classList.toggle("askalcf-in-toc", visible);
  }

  // ---- build DOM ---------------------------------------------------------
  function build() {
    var root = document.createElement("div");
    root.id = "askalcf-root";

    var fab = document.createElement("button");
    fab.id = "askalcf-fab";
    fab.type = "button";
    fab.setAttribute("aria-label", "Open AskALCF chat");
    fab.innerHTML = LAUNCHER_ICON;
    fab.addEventListener("click", function () { setOpen(true); });

    var panel = document.createElement("div");
    panel.id = "askalcf-panel";
    panel.setAttribute("role", "dialog");
    panel.setAttribute("aria-label", "AskALCF chat");

    var header = document.createElement("div");
    header.className = "askalcf-header";
    header.innerHTML =
      '<div class="askalcf-head-title">' + CHAT_ICON +
      '<div><div class="askalcf-title">AskALCF</div>' +
      '<div class="askalcf-sub">ALCF User Guide assistant</div></div></div>' +
      '<div class="askalcf-spacer"></div>' +
      '<button class="askalcf-iconbtn" id="askalcf-max" title="Maximize" aria-label="Maximize" aria-pressed="false">' + MAX_ICON + '</button>' +
      '<button class="askalcf-iconbtn" id="askalcf-clear" title="New chat" aria-label="New chat">' + CLEAR_ICON + '</button>' +
      '<button class="askalcf-iconbtn" id="askalcf-close" title="Close" aria-label="Close">' + CLOSE_ICON + '</button>';

    var messages = document.createElement("div");
    messages.className = "askalcf-messages";

    var inputRow = document.createElement("div");
    inputRow.className = "askalcf-input-row";
    var input = document.createElement("textarea");
    input.rows = 1;
    input.placeholder = "Ask about ALCF…";
    input.setAttribute("aria-label", "Message");
    var sendBtn = document.createElement("button");
    sendBtn.className = "askalcf-send";
    sendBtn.type = "button";
    sendBtn.textContent = "Send";
    inputRow.appendChild(input);
    inputRow.appendChild(sendBtn);

    var disclaimer = document.createElement("div");
    disclaimer.className = "askalcf-disclaimer";
    disclaimer.textContent = "AskALCF can make mistakes. Verify important details in the docs.";

    var live = document.createElement("div");
    live.className = "askalcf-sr-only";
    live.setAttribute("aria-live", "polite");

    panel.appendChild(header);
    panel.appendChild(messages);
    panel.appendChild(inputRow);
    panel.appendChild(disclaimer);
    root.appendChild(fab);
    root.appendChild(panel);
    root.appendChild(live);  // outside the panel, so it still speaks when the panel is closed
    document.body.appendChild(root);

    els = { root: root, fab: fab, panel: panel, messages: messages, input: input, send: sendBtn, live: live,
            max: header.querySelector("#askalcf-max") };

    // events
    sendBtn.addEventListener("click", send);
    els.max.addEventListener("click", function () { setMax(!state.max); });
    input.addEventListener("keydown", function (e) {
      // Enter also confirms text in CJK input methods; Safari reports
      // isComposing false for that Enter, but keyCode 229.
      if (e.isComposing || e.keyCode === 229) return;
      if (e.key === "Enter" && !e.shiftKey) { e.preventDefault(); send(); }
    });
    panel.addEventListener("keydown", function (e) {
      // Keys typed in the panel stay here: with Material's search open, its
      // handler would move them to the search box, and Escape would close it.
      e.stopPropagation();
      // Escape also cancels an IME conversion; only a plain Escape closes.
      if (e.key === "Escape" && !e.isComposing && e.keyCode !== 229) setOpen(false);
    });
    input.addEventListener("input", autoSize);
    header.querySelector("#askalcf-close").addEventListener("click", function () { setOpen(false); });
    header.querySelector("#askalcf-clear").addEventListener("click", function () {
      generation++;
      clearTimeout(announceTimer);
      els.live.textContent = "";
      setBusy(false);
      state.messages = [];
      state.conversationId = null;
      save();
      render();
    });
    // Instant navigation swaps the page, and with it the TOC column.
    window.addEventListener("resize", placeLauncher);
    if (window.document$) window.document$.subscribe(placeLauncher);
    else placeLauncher();

    render();
    if (state.max) setMax(true);
    if (state.open) setOpen(true, false);
  }

  function init() {
    load();
    build();
  }
  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", init);
  } else {
    init();
  }
})();
