/* AskALCF floating chat widget.
 *
 * A floating chat bubble that talks to the AskALCF conversational agent
 * (RAG over the ALCF docs + public HPC tools). Mirrors the behavior of the
 * Streamlit app_convo_agent: tool-call display, source citations, multi-turn
 * conversation. Anonymous in v1 (no auth).
 *
 * Endpoint is configurable via window.ASKALCF_API_BASE (set in
 * overrides/main.html). Falls back to localhost for local development.
 *
 * Built once and appended to <body>, so it survives Material's instant
 * navigation (the body element persists; only the content region is swapped).
 */
(function () {
  "use strict";
  if (window.__askalcfWidgetInit) return;
  window.__askalcfWidgetInit = true;

  var API_BASE = (window.ASKALCF_API_BASE || "http://localhost:8080").replace(/\/+$/, "");
  var ENDPOINT = API_BASE + "/v1/agent/chat";
  var LOGO = window.ASKALCF_LOGO_URL || "/images/icon-for-chat-window.png";
  var STORE_KEY = "askalcf:v1";

  var CHAT_ICON =
    '<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" ' +
    'stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">' +
    '<path d="M21 11.5a8.5 8.5 0 0 1-12.3 7.6L3 21l1.9-5.7A8.5 8.5 0 1 1 21 11.5z"/>' +
    '<circle cx="8.5" cy="12" r="1.1" fill="currentColor" stroke="none"/>' +
    '<circle cx="12" cy="12" r="1.1" fill="currentColor" stroke="none"/>' +
    '<circle cx="15.5" cy="12" r="1.1" fill="currentColor" stroke="none"/>' +
    "</svg>";

  var state = { messages: [], conversationId: null, open: false };
  var busy = false;
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
  function renderMarkdown(text) {
    if (window.marked && window.DOMPurify) {
      try {
        var html = window.marked.parse(String(text || ""), { breaks: true });
        return window.DOMPurify.sanitize(html, { ADD_ATTR: ["target", "rel"] });
      } catch (e) { /* fall through */ }
    }
    return escapeHtml(text).replace(/\n/g, "<br>");
  }

  // ---- sources & tool calls ---------------------------------------------
  function titleFromSource(src) {
    if (!src) return "";
    var s = String(src).split(/[?#]/)[0].replace(/\/+$/, "");
    var last = s.split("/").pop() || s;
    return last.replace(/\.(md|html?|txt)$/i, "").replace(/[-_]/g, " ").trim();
  }
  function renderSources(contexts) {
    if (!contexts || !contexts.length) return "";
    var seen = {}, items = [];
    contexts.forEach(function (c) {
      var md = (c && c.metadata) || {};
      var src = c.source || md.source || "";
      var url = md.url || md.source_url || md.link || "";
      if (!url && /^https?:\/\//i.test(src)) url = src;
      var title = md.title || md.page_title || titleFromSource(url || src) || "Source";
      var key = (url || title).toLowerCase();
      if (seen[key]) return;
      seen[key] = true;
      if (url) {
        items.push('<li><a href="' + escapeHtml(url) + '" target="_blank" rel="noopener">' +
          escapeHtml(title) + "</a></li>");
      } else {
        items.push("<li>" + escapeHtml(title) + "</li>");
      }
    });
    if (!items.length) return "";
    return '<details class="askalcf-sources"><summary>Sources (' + items.length +
      ")</summary><ol>" + items.join("") + "</ol></details>";
  }
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
  function render() {
    var box = els.messages;
    box.innerHTML = "";

    if (!state.messages.length) {
      var w = document.createElement("div");
      w.className = "askalcf-welcome";
      w.innerHTML = '<img src="' + LOGO + '" alt="AskALCF">' +
        "<strong>Welcome to AskALCF</strong>Ask about ALCF systems,<br>" +
        "software, jobs, and documentation.<br>Ask me anything to get started.";
      box.appendChild(w);
    }

    state.messages.forEach(function (m) {
      var row = document.createElement("div");
      row.className = "askalcf-msg " + (m.role === "user" ? "user" : "assistant");
      var bubble = document.createElement("div");
      bubble.className = "askalcf-bubble";
      if (m.role === "user") {
        bubble.textContent = m.content;
      } else if (m.error) {
        bubble.innerHTML = '<span class="askalcf-error">Sorry, something went wrong: ' +
          escapeHtml(m.error) + "</span>";
      } else {
        bubble.innerHTML = renderToolCalls(m.tool_calls) +
          renderMarkdown(m.content) +
          renderSources(m.contexts);
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
    els.input.disabled = v;
  }

  // ---- networking --------------------------------------------------------
  function send() {
    var text = els.input.value.trim();
    if (!text || busy) return;
    els.input.value = "";
    autoSize();
    state.messages.push({ role: "user", content: text });
    setBusy(true);
    render();
    save();

    var payload = {
      messages: state.messages.map(function (m) {
        return { role: m.role, content: m.content };
      }),
      conversation_id: state.conversationId || null
    };

    fetch(ENDPOINT, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(payload)
    }).then(function (res) {
      if (!res.ok) throw new Error("HTTP " + res.status);
      return res.json();
    }).then(function (data) {
      state.conversationId = data.conversation_id || state.conversationId;
      state.messages.push({
        role: "assistant",
        content: data.content || "",
        tool_calls: data.tool_calls || [],
        contexts: data.contexts || []
      });
    }).catch(function (err) {
      state.messages.push({ role: "assistant", content: "", error: String(err.message || err) });
    }).then(function () {
      setBusy(false);
      render();
      save();
    });
  }

  // ---- input sizing ------------------------------------------------------
  function autoSize() {
    var ta = els.input;
    ta.style.height = "auto";
    ta.style.height = Math.min(ta.scrollHeight, 120) + "px";
  }

  // ---- open/close --------------------------------------------------------
  function setOpen(v) {
    state.open = v;
    els.root.classList.toggle("askalcf-open", v);
    save();
    if (v) setTimeout(function () { els.input.focus(); }, 50);
  }

  // ---- build DOM ---------------------------------------------------------
  function build() {
    var root = document.createElement("div");
    root.id = "askalcf-root";

    var fab = document.createElement("button");
    fab.id = "askalcf-fab";
    fab.type = "button";
    fab.setAttribute("aria-label", "Open AskALCF chat");
    // The robot icon is a CSS background, so it can follow the color scheme.
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
      '<button class="askalcf-iconbtn" id="askalcf-clear" title="New chat" aria-label="New chat">↻</button>' +
      '<button class="askalcf-iconbtn" id="askalcf-close" title="Close" aria-label="Close">✕</button>';

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

    panel.appendChild(header);
    panel.appendChild(messages);
    panel.appendChild(inputRow);
    panel.appendChild(disclaimer);
    root.appendChild(fab);
    root.appendChild(panel);
    document.body.appendChild(root);

    els = { root: root, fab: fab, panel: panel, messages: messages, input: input, send: sendBtn };

    // events
    sendBtn.addEventListener("click", send);
    input.addEventListener("keydown", function (e) {
      if (e.key === "Enter" && !e.shiftKey) { e.preventDefault(); send(); }
    });
    input.addEventListener("input", autoSize);
    header.querySelector("#askalcf-close").addEventListener("click", function () { setOpen(false); });
    header.querySelector("#askalcf-clear").addEventListener("click", function () {
      state.messages = [];
      state.conversationId = null;
      save();
      render();
    });

    render();
    if (state.open) setOpen(true);
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
