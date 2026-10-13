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

  // Color for a Last updated date, by age in days.
  var AGE_BANDS = [[180, "fresh"], [365, "aging"], [730, "old"], [Infinity, "stale"]];
  function ageBand(days) {
    for (var i = 0; i < AGE_BANDS.length; i++) if (days <= AGE_BANDS[i][0]) return AGE_BANDS[i][1];
  }
  function daysBetween(from, to) {
    return Math.round((new Date(to) - new Date(from)) / 86400000);
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
    var owners = {};
    pages.forEach(function (p) {
      (p.owners.length ? p.owners : ["(no owner)"]).forEach(function (o) { owners[o] = true; });
    });
    var NOT_IN_NAV = "(not in sidebar)";
    function sectionPath(p) { return p.section.length ? p.section : [NOT_IN_NAV]; }

    var filters = {
      status: el("select", {}, [option("", "All pages"), option("stale", "Stale"),
        option("never", "Never verified"), option("ok", "Verified"), option("missing", "No description")]),
      // One select per level of the chosen section path, added as you go.
      section: el("span", { class: "alcf-page-status__sections" }),
      owner: el("select", {}, [option("", "All owners")].concat(
        Object.keys(owners).sort().map(function (o) { return option(o, o); }))),
      search: el("input", { type: "search", placeholder: "Filter by title or path" }),
      nav: el("input", { type: "checkbox" })
    };
    var sectionPrefix = [];  // the chosen section path so far
    function selectSection(path) {
      sectionPrefix = path.slice();
      buildSectionSelects();
      update();
    }
    function buildSectionSelects() {
      var selects = [];
      for (var level = 0; level <= sectionPrefix.length; level++) {
        var prefix = sectionPrefix.slice(0, level);
        var names = {};
        pages.forEach(function (p) {
          var path = sectionPath(p);
          if (path.length > level && prefix.every(function (n, i) { return path[i] === n; })) names[path[level]] = true;
        });
        var keys = Object.keys(names).sort();
        if (!keys.length) break;
        var select = el("select", {}, [option("", level ? "All subsections" : "All sections")].concat(
          keys.map(function (n) { return option(n, n); })));
        select.value = sectionPrefix[level] || "";
        select.addEventListener("change", (function (level) {
          return function (e) {
            sectionPrefix = sectionPrefix.slice(0, level);
            if (e.target.value) sectionPrefix.push(e.target.value);
            buildSectionSelects();
            update();
          };
        })(level));
        selects.push(select);
      }
      filters.section.replaceChildren.apply(filters.section, selects);
    }
    buildSectionSelects();
    var summary = el("div", { class: "alcf-page-status__summary" });
    var bar = el("div", { class: "alcf-page-status__filters" }, [
      filters.status, filters.section, filters.owner, filters.search,
      el("label", {}, [filters.nav, document.createTextNode(" Include pages not in the sidebar")])
    ]);
    var tableWrap = el("div", { class: "md-typeset__table" });
    // Buttons, not anchors: Material's instant navigation reloads the page
    // for an anchor with href="#", which resets the table.
    tableWrap.addEventListener("click", function (e) {
      var button = e.target.closest("button[data-section]");
      if (button) selectSection(JSON.parse(button.dataset.section));
    });
    root.replaceChildren(summary, bar, tableWrap);

    // Each part of the section path filters the table to that path.
    function sectionLinks(section) {
      var nodes = [];
      section.forEach(function (name, i) {
        if (i) nodes.push(document.createTextNode(" / "));
        nodes.push(el("button", { type: "button", class: "alcf-page-status__link", "data-section": JSON.stringify(section.slice(0, i + 1)), text: name }));
      });
      return nodes;
    }
    function ownerLinks(owners) {
      var nodes = [];
      owners.forEach(function (o, i) {
        if (i) nodes.push(document.createTextNode(" "));
        nodes.push(el("a", { href: "https://github.com/" + o.replace(/^@/, ""), text: o }));
      });
      return nodes;
    }
    function dateKey(date) {  // YYYY-MM-DD as a sortable integer; blanks sort first
      return date ? date.replace(/-/g, "") : 0;
    }
    function updatedCell(updated) {
      if (!updated) return el("td");
      var days = daysBetween(updated, data.generated);
      return el("td", { class: "alcf-page-status__age--" + ageBand(days), title: days + " days ago", text: updated, "data-sort": dateKey(updated) });
    }

    function update() {
      var shown = pages.filter(function (p) {
        var s = status(p)[0];
        if (!filters.nav.checked && !p.in_nav) return false;
        if (filters.status.value === "missing" ? p.description : filters.status.value && s !== filters.status.value) return false;
        var path = sectionPath(p);
        if (!sectionPrefix.every(function (n, i) { return path[i] === n; })) return false;
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

      // Dates and counts sort as numbers (data-sort): Tablesort's date plugin
      // doesn't recognize YYYY-MM-DD and sorts those by day of month.
      var NUMERIC = { Created: 1, Updated: 1, Verified: 1, Status: 1, Description: 1, "Links out": 1, "Links in": 1 };
      var head = el("tr", {}, ["Page", "Section", "Owners", "Created", "Updated", "Verified", "Status", "Description", "Links out", "Links in"]
        .map(function (h) { return el("th", NUMERIC[h] ? { text: h, "data-sort-method": "number" } : { text: h }); }));
      var rows = shown.map(function (p) {
        var st = status(p);
        // Stale first (by age), then never verified, then verified (by age).
        var statusKey = p.stale ? p.verified_age_days : p.last_verified ? 2000000 + p.verified_age_days : 1000000;
        return el("tr", {}, [
          el("td", { class: "alcf-page-status__page" }, [
            el("a", { href: new URL(p.url, base).href, text: p.title }),
            el("br"), el("small", { text: p.path })
          ]),
          el("td", {}, sectionLinks(p.section)),
          el("td", {}, ownerLinks(p.owners)),
          el("td", { text: p.created || "", "data-sort": dateKey(p.created) }),
          updatedCell(p.updated),
          el("td", { text: p.last_verified || "", "data-sort": dateKey(p.last_verified) }),
          el("td", { class: "alcf-page-status__status--" + st[0], text: st[1], "data-sort": statusKey }),
          el("td", { class: p.description ? "" : "alcf-page-status__status--missing", text: p.description ? "yes, " + p.description_length + " chars" : "none", "data-sort": p.description_length }),
          el("td", { text: p.links.length }),
          el("td", { text: p.inbound })
        ]);
      });
      var table = el("table", {}, [el("thead", {}, [head]), el("tbody", {}, rows)]);
      tableWrap.replaceChildren(table);
      if (typeof Tablesort !== "undefined") new Tablesort(table);
    }

    ["status", "owner", "search", "nav"].forEach(function (k) {
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
