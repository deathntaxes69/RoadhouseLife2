function formatForEnfusion(data) {
  let results = [];
  if (Array.isArray(data)) {
    results = data;
  } else if (data) {
    results = [data];
  }

  // This is the correct format for the game's low-level stream parser.
  const header = "EPF_JSON_STREAM_START\n";
  const footer = "\nEPF_JSON_STREAM_END";
  const body = results.map(item => JSON.stringify(item)).join('\n');

  return header + body + footer;
}

module.exports = { formatForEnfusion };