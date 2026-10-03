function applyTablesort() {
  var tables = document.querySelectorAll("article table:not([class])");

  tables.forEach(function(table) {
    if (table.dataset.tablesortInitialized) return; // skip if already set

    new Tablesort(table);
    table.dataset.tablesortInitialized = "true"; // flag as initialized
  });
}

// Internal navigation (MkDocs Material)
if (typeof document$ !== "undefined") {
  document$.subscribe(applyTablesort);
}

// Full load
document.addEventListener("DOMContentLoaded", applyTablesort);

// DOM already loaded (standalone pages)
if (document.readyState === "interactive" || document.readyState === "complete") {
  applyTablesort();
}
