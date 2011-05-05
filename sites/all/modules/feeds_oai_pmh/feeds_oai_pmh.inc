<?php


/**
 * Functions to connect and process data from OAI-PMH repositories.
 * See http://www.openarchives.org/OAI/openarchivesprotocol.html
 */

/**
 * Returns an array of information returned by the OAI-PMH Identify verb.
 */
function feeds_oai_pmh_identify($baseurl) {
  static $cache = array();
  if ($cache[$baseurl]) {
    return $cache[$baseurl];
  }
  $output = array();
  $url = "$baseurl?verb=Identify";
  $repository = array(
    'deleted_record' => '',
    'compression' => FALSE,
    'compression_gzip' => FALSE,
    'compression_deflate' => FALSE,
    'earliest_timestamp' => '',
    'sets' => array(),
  );

  $result = drupal_http_request($url);
  if ($result->code != 200) {
    $message = t('OAI repository %repo at url %url is not avaliable, please check URL is correct.', array('%repo' => $baseurl, '%url' => $baseurl));
    watchdog('feeds_oai_pmh', $message, '', WATCHDOG_ERROR);
    return array(
      'output' => $message,
      'status' => 1,
    );
  }

  // Returns FALSE on error
  $xml = @simplexml_load_string($result->data);
  if (!$xml) {
    $message = t('OAI repository %repo returns invalid XML upon identify.', array('%repo' => $baseurl));
    watchdog('feeds_oai_pmh', $message, '', WATCHDOG_ERROR);
    return array(
      'output' => $message,
      'status' => 1,
    );
  }

  $ident = $xml->Identify;
  // Things which must come back, or die
  // Protocool Version
  if ($ident->protocolVersion != '2.0') {
    $message = t('OAI repository %repo: Incorrect Identify Response -- Unsupported Protcool Version "@version"', array('%repo' => $baseurl, '@version' => $ident->protocolVersion));
    watchdog('feeds_oai_pmh', $message, '', WATCHDOG_ERROR);
    return array(
      'output' => $message,
      'status' => 1,
    );
  }
  else {
    $repository["protocol_version"] = (string)$ident->protocolVersion;
  }

  // DeleteRecord
  if (!isset($ident->deletedRecord)) {
    $message = t('OAI repository %repo: Incorrect Identify Response -- No deleteRecord', array('%repo' => $baseurl));
    watchdog('feeds_oai_pmh', $message, '', WATCHDOG_ERROR);
    return array(
      'output' => $message,
      'status' => 1,
    );
  }
  else {
    $repository['deleted_record'] = (string)$ident->deletedRecord;
  }

  // earliest Datestamp
  if (!isset($ident->earliestDatestamp)) {
    $message = t('OAI repository %repo: Incorrect Identify Response -- No earliest Datestamp', array('%repo' => $baseurl));
    watchdog('feeds_oai_pmh', $message, '', WATCHDOG_ERROR);
    return array(
      'output' => $message,
      'status' => 1,
    );
  }
  else {
    #$repository['earliest_datestamp'] = (string)$ident->earliestDatestamp;
    $repository['earliest_timestamp'] = strtotime((string)$ident->earliestDatestamp);
  }

  // Granularity
  if (!isset($ident->granularity)) {
    $message = t('OAI repository %repo: Incorrect Identify Response -- No Granularity', array('%repo' => $baseurl));
    watchdog('feeds_oai_pmh', $message, '', WATCHDOG_ERROR);
    return array(
      'output' => $message,
      'status' => 1,
    );

  }
  else {
    // Granularty is only in days
    // Magic number from strlen(YYYY-MM-DD)
    if (strlen($ident->granularity) == 10) {
      $repository['granularity'] = 'days';
    }
    // Granularity is in seconds
    // Magic number from strlen(YYYY-MM-DDThh:mm:ssZ)
    elseif (strlen($ident->granularity) == 20) {
      $repository['granularity'] = 'seconds';
    }
    else {
      $message = t('OAI repository %repo: Incorrect Identify Response -- Invalid granularity', array('%repo' => $baseurl));
      watchdog('feeds_oai_pmh', $message, '', WATCHDOG_ERROR);
      return array(
        'output' => $message,
        'status' => 1,
      );
    }
  }

  // Optional things, which are nice to have
  if (isset($ident->compression)) {
    // According to HTTP 1.1 RFC 2616 there is also the Lempel-Ziv-Welch
    // compression, which in theory could be supported. However, PHP doesn't
    // seem to play nice with it, and I haven't seen a repo with it. It is also
    // 14 years old.
    $repository['compression'] = TRUE;
    foreach ($ident->compression as $encoding) {
      if ($encoding == 'gzip') {
        $repository['compression_gzip'] = TRUE;
      }
      elseif ($encoding == 'deflate') {
        $repository['compression_deflate'] = TRUE;
      }
    }
  }

  // Get and assign sets information
  if ($sets = feeds_oai_pmh_get_sets($baseurl)) {
    $repository['sets'] = $sets;
  }

  $return = array('output' => $output, 'status' => 0, 'repository' => $repository);
  $cache[$baseurl] = $return;
  return $return;
}

/**
 * Returns an array populated with the avaiable sets reported by an OAI-PMH endpoint.
 */
function feeds_oai_pmh_get_sets($baseurl) {
  $sets = array();
  $url = "$baseurl?verb=ListSets";
  $result = drupal_http_request($url);

  // Return false on error
  if ($result->code != 200) {
    return FALSE;
  }
  $xml = simplexml_load_string($result->data);
  if (!$xml) {
    return FALSE;
  }
  if (isset($xml->error)) {
    return FALSE;
  }

  // Put set names into $sets array
  foreach ($xml->ListSets->set as $set) {
    $sets[(string)$set->setSpec]['name'] = (string)$set->setName;
    if ($set->setDescription) {
      // TODO: Use SimpleXML instead of regexp
      $description = $set->setDescription->asXML();
      $description = preg_replace('/.*?<dc:description>([^<]+)<.dc:description.*/s', '\1', $description);
      $sets[(string)$set->setSpec]['description'] = $description;
    }
  }
  return $sets;
}

/**
 * Parse a raw response from an OAI-PMH endpoint into an array of items.
 */
function feeds_oai_pmh_parse($raw_xml) {
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

    $xml_dc_metadata = $xml_item->metadata->children('http://www.openarchives.org/OAI/2.0/oai_dc/')->children('http://purl.org/dc/elements/1.1/');
    $item = array(
      'guid' => (string)$xml_item->header->identifier,
      'datestamp' => strtotime((string)$item->header->datestamp),
      'title' => (string)$xml_dc_metadata->title,
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
      'creator', 'subject', 'description', 'publisher', 'contributor',
      'date', 'type', 'format', 'identifier', 'source', 'language', 'relation',
      'coverage', 'rights');
    foreach ($elements as $element) {
      if (isset($xml_dc_metadata->$element)) {
        $item[$element] = array();
        foreach ($xml_dc_metadata->$element as $value) {
          $value = (string)$value;
          // TODO: add callback functions to further process values (like convert dates to timestamps, split subjects, etc.)
          $item[$element][$value] = $value;
        }
        $item[$element] = array_values($item[$element]);
      }
    }

    // Add "url" element from "identifier" items that are URLs.
    foreach ($item['identifier'] as $value) {
      if (valid_url($value, TRUE)) {
        $item['url'][] = $value;
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