function applyTablesort() {
  var tables = document.querySelectorAll("article table:not([class])");
  tables.forEach(function(table) {
    new Tablesort(table);
  });
}

// If document$ is available (MkDocs Material instant navigation), subscribe
if (typeof document$ !== "undefined") {
  document$.subscribe(applyTablesort);
} else {
  // Otherwise just run on DOMContentLoaded (standalone pages, or full reload)
  document.addEventListener("DOMContentLoaded", applyTablesort);
}
