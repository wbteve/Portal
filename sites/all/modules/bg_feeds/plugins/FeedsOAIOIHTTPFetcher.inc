<?php


/**
 * Definition of the import batch object created on the fetching stage by
 * FeedsOAIOIHTTPFetcher.
 */
class FeedsOAIOIHTTPFetcherResult extends FeedsOAIHTTPFetcherResult {
  /**
   * Implementation of FeedsResult::getRaw();
   */
  public function getRaw() {
    // TODO: Move the URL building and data fetching to feeds_oai_pmh.inc
    // Build the request URL
    $url = $this->oai_endpoint_url;
    $url .= '?verb=ListRecords';
    if ($this->resumption_token) {
      $url .= "&resumptionToken=" . rawurldecode($this->resumption_token);
    }
    else {
      // When a resumptionToken is issued, there can't be any other arguments
      // in the request.
      $url .= '&metadataPrefix=oai_oi';
      if ($this->from_timestamp > 0) {
        $url .= '&from=' . rawurlencode($this->formatDate($this->from_timestamp));
      }
      if ($this->until_timestamp > 0) {
        $url .= '&until=' . rawurlencode($this->formatDate($this->until_timestamp));
      }
      if ($this->set) {
        $url .= '&set=' . rawurlencode($this->set);
      }
    }

    #dsm("URL for OAI request: $url");

    // Fetch the feed's contents
    $result = drupal_http_request($url);

    if ($result->code == 200) {
      // TODO: Use simpleXML instead of regexp
      // Try to get resumptionToken. Example:
      // <resumptionToken completeListSize="478" cursor="0">0/300/478/oai_dc/eserev/null/null</resumptionToken>
      $ok = preg_match_all('/<resumptionToken.*?>([^<]+)<\/resumptionToken>/s', $result->data, $matches);
      if ($ok) {
        $resumption_token = array_pop($matches[1]);
        #dsm("Resumption token: $resumption_token");
        $this->setLastDate(0);
      }
      else {
        // No resumption token in response. Store current system timestamp
        // so next request limits items returned.
        $resumption_token = "";
        $this->setLastDate(time());
      }
      $this->setResumptionToken($resumption_token);
    }
    else {
      // OAI fetch failed
      #dsm("OAI fetch failed with error : " . $result->error);
      // TODO: Log via watchdog?
    }
    // Return the feed's contents
    return $result->data;
  }
}


/**
 * Fetcher class for OAI-PMH Open Image repository webservices.
 */
class FeedsOAIOIHTTPFetcher extends FeedsOAIHTTPFetcher {
  /**
   * Override parent::sourceForm
   */
  public function sourceForm($source_config) {
    $form = parent::sourceForm($source_config);

    $form['source']['#ajax']['callback'] = 'feeds_oai_oi_ajax_callback';

    return $form;
  }
}
