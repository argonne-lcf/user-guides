/* Page status dashboard (docs/page-status.md): renders page-metadata.json,
   written by hooks/page_metadata.py, as a filterable table. Runs only on a
   page with a .alcf-page-status element; re-runs after instant navigation
   (document$). */
(function () {
  function el(tag, attrs, children) {
    var node = document.createElement(tag);
    Object.keys(attrs || {}).forEach(function (k) {
      if (k === "text") node.textContent = attrs[k];
      else node.setAttribute(k, attrs[k]);
    });
    (children || []).forEach(function (c) { node.appendChild(c); });
    return node;
  }

  function option(value, text) {
    return el("option", { value: value, text: text });
  }

  function status(page) {
    if (page.stale === "age") return ["stale", "Stale: " + page.verified_age_days + " days"];
    if (page.stale === "updated") return ["stale", "Stale: changed " + page.updated];
    if (page.last_verified) return ["ok", "Verified " + page.verified_age_days + " days ago"];
    return ["never", "Never verified"];
  }

  function render(root, data) {
    var pages = data.pages;
    var base = new URL(root.dataset.src, location.href);  // site root, for page links
    var sections = {}, owners = {};
    pages.forEach(function (p) {
      sections[p.section[0] || "(not in sidebar)"] = true;
      (p.owners.length ? p.owners : ["(no owner)"]).forEach(function (o) { owners[o] = true; });
    });

    var filters = {
      status: el("select", {}, [option("", "All pages"), option("stale", "Stale"),
        option("never", "Never verified"), option("ok", "Verified"), option("missing", "No description")]),
      section: el("select", {}, [option("", "All sections")].concat(
        Object.keys(sections).sort().map(function (s) { return option(s, s); }))),
      owner: el("select", {}, [option("", "All owners")].concat(
        Object.keys(owners).sort().map(function (o) { return option(o, o); }))),
      search: el("input", { type: "search", placeholder: "Filter by title or path" }),
      nav: el("input", { type: "checkbox" })
    };
    var summary = el("div", { class: "alcf-page-status__summary" });
    var bar = el("div", { class: "alcf-page-status__filters" }, [
      filters.status, filters.section, filters.owner, filters.search,
      el("label", {}, [filters.nav, document.createTextNode(" Include pages not in the sidebar")])
    ]);
    var tableWrap = el("div", { class: "md-typeset__table" });
    root.replaceChildren(summary, bar, tableWrap);

    function update() {
      var shown = pages.filter(function (p) {
        var s = status(p)[0];
        if (!filters.nav.checked && !p.in_nav) return false;
        if (filters.status.value === "missing" ? p.description : filters.status.value && s !== filters.status.value) return false;
        if (filters.section.value && (p.section[0] || "(not in sidebar)") !== filters.section.value) return false;
        if (filters.owner.value && (p.owners.length ? p.owners : ["(no owner)"]).indexOf(filters.owner.value) < 0) return false;
        var q = filters.search.value.trim().toLowerCase();
        if (q && (p.title + " " + p.path).toLowerCase().indexOf(q) < 0) return false;
        return true;
      });
      var counts = { stale: 0, never: 0, ok: 0, missing: 0 };
      shown.forEach(function (p) {
        counts[status(p)[0]]++;
        if (!p.description) counts.missing++;
      });
      summary.replaceChildren(
        el("span", {}, [el("strong", { text: shown.length }), document.createTextNode("pages")]),
        el("span", {}, [el("strong", { text: counts.ok }), document.createTextNode("verified")]),
        el("span", {}, [el("strong", { text: counts.stale }), document.createTextNode("stale")]),
        el("span", {}, [el("strong", { text: counts.never }), document.createTextNode("never verified")]),
        el("span", {}, [el("strong", { text: counts.missing }), document.createTextNode("without a description")]),
        el("span", {}, [document.createTextNode("Data from " + data.generated + "; stale after " + data.last_verified_max_age_days + " days")])
      );

      var head = el("tr", {}, ["Page", "Section", "Owners", "Created", "Updated", "Verified", "Status", "Description", "Links out", "Links in"]
        .map(function (h) { return el("th", { text: h }); }));
      var rows = shown.map(function (p) {
        var st = status(p);
        var sortKey = p.stale ? 0 : p.last_verified ? 2 : 1;
        return el("tr", {}, [
          el("td", { class: "alcf-page-status__page" }, [
            el("a", { href: new URL(p.url, base).href, text: p.title }),
            el("br"), el("small", { text: p.path })
          ]),
          el("td", { text: p.section.join(" / ") }),
          el("td", { text: p.owners.join(" ") }),
          el("td", { text: p.created || "" }),
          el("td", { text: p.updated || "" }),
          el("td", { text: p.last_verified || "" }),
          el("td", { class: "alcf-page-status__status--" + st[0], text: st[1], "data-sort": sortKey + " " + (p.verified_age_days || 0) }),
          el("td", { class: p.description ? "" : "alcf-page-status__status--missing", text: p.description ? "yes, " + p.description_length + " chars" : "none", "data-sort": p.description_length }),
          el("td", { text: p.links.length }),
          el("td", { text: p.inbound })
        ]);
      });
      var table = el("table", {}, [el("thead", {}, [head]), el("tbody", {}, rows)]);
      tableWrap.replaceChildren(table);
      if (typeof Tablesort !== "undefined") new Tablesort(table);
    }

    Object.keys(filters).forEach(function (k) {
      filters[k].addEventListener(k === "search" ? "input" : "change", update);
    });
    update();
  }

  function init() {
    var root = document.querySelector(".alcf-page-status");
    if (!root || root.dataset.loaded) return;
    root.dataset.loaded = "true";
    fetch(root.dataset.src).then(function (r) {
      if (!r.ok) throw new Error(r.status + " " + r.statusText);
      return r.json();
    }).then(function (data) {
      render(root, data);
    }).catch(function (err) {
      root.replaceChildren(el("p", { text: "Couldn't load page-metadata.json: " + err.message }));
    });
  }

  if (typeof document$ !== "undefined") document$.subscribe(init);
  else document.addEventListener("DOMContentLoaded", init);
})();
