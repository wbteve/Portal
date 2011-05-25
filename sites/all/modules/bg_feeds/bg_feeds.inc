<?php
/**
 * Parse a raw response from an OAI-PMH Open Images endpoint into an array of items.
 */
function bg_feeds_oai_oi_parse($raw_xml) {
  // Items array
  $items = array();

  // Parse raw string into xml
  $xml = simplexml_load_string($raw_xml);
  if (!$xml) {
    return FALSE;
  }

  // If error element is set, we have a problem. Blow up before the
  // foreach blows up for us. More info this way too.
  if (isset($xml->error)) {
    return FALSE;
  }

  foreach ($xml->ListRecords->record as $xml_item) {
    // TODO: Handle items marked "deleted" in repository, if so configured.
    // TODO: Handle updates to existing nodes.

    // Skip items marked "deleted"
    if ($xml_item->header["status"] == "deleted") {
      continue;
    }

    $xml_oi_metadata = $xml_item->metadata->children('oai_oi', TRUE)->children('oi', TRUE);
    $item = array(
      'guid' => (string)$xml_item->header->identifier,
      'datestamp' => strtotime((string)$xml_item->header->datestamp),
      'title' => (string)$xml_oi_metadata->title,
    );

    // Parse the setSpec from each item's header
    // Some implementations might repeat the same setSpec, so de-dupe.
    $set_spec_values = array();
    foreach ($xml_item->header->setSpec as $value) {
      $value = (string)$value;
      $set_spec_values[$value] = $value;
    }
    $item['setspec_raw'] = array_values($set_spec_values);

    // Parse elements
    $elements = array(
      #'title',
      'alternative', 'creator', 'subject',  'description', 'abstract', 
      'publisher', 'contributor', 'date', 'type', 'extent', 'medium',
      'identifier', 'source', 'language', 'references', 'spatial', 'attributionName',
      'attributionURL', 'license');
    foreach ($elements as $element) {
      if (isset($xml_oi_metadata->$element)) {
        $item[$element] = array();

        foreach ($xml_oi_metadata->$element as $value) {
          // @TODO Rewrite to xpath to select only the items needed
          $xml_ns = $value->attributes("xml", 1);
          $lang   = (string)$xml_ns['lang'];

          if(in_array($lang, array('', 'nl'))) {
            $value  = (string)$value;

            // TODO: add callback functions to further process values (like convert dates to timestamps, split subjects, etc.)
            $item[$element][$value] = $value;
          }
        }

        $item[$element] = array_values($item[$element]);
      }
    }

    // Add "url" element from "identifier" items that are URLs.
    if(isset($item['identifier']) && is_array($item['identifier'])) {
      foreach ($item['identifier'] as $value) {
        if (valid_url($value, TRUE)) {
          $item['url'][] = $value;
        }
      }
    }

    $items[] = $item;
  }

  // if a resumption token is set, and it is non-null. Requests with
  // resumptionTokens come back with an empty self closing tag
  // indicating the end of the request.
  //  if (
  //    isset($xml->ListRecords->resumptionToken)
  //    && (string)$xml->ListRecords->resumptionToken != ''
  //    ) {
  //
  //    $resumption_token = (string)$xml->ListRecords->resumptionToken;
  //    dsm("Resumption token: " . $resumption_token);
  //    // Run the loop a second time, update the request url
  //    #$request = '?verb=ListRecords&resumptionToken='.
  //    #_oai_pmh_clean_url((string)$xml->ListRecords->resumptionToken);
  //    // Unneeded in theory, but makes me feel better
  //    #unset($xml->ListRecords->resumptionToken);
  //    #dsm("Next request will be: $request");
  //    #$times++;
  //    #if ($times == 2 ) {
  //    #  dsm("Looped $times times, breaking.");
  //    #  break;
  //    #}
  //  }

  #dsm("All the items returned:");
  #dpm($items);
  return array(
    'items' => $items,
  #'resumptionToken' => $resumption_token,
  );
}