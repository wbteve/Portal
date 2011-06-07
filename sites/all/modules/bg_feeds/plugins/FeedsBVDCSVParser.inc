<?php
/**
 * Parses a given file as a CSV file.
 */
class FeedsBVDCSVParser extends FeedsCSVParser {
  /**
   * Implements FeedsParser::parse().
   */
  public function parse(FeedsSource $source, FeedsFetcherResult $fetcher_result) {
    $source_config = $source->getConfigFor($this);
    $state = $source->state(FEEDS_PARSE);

    // Load and configure parser.
    feeds_include_library('ParserCSV.inc', 'ParserCSV');
    $parser = new ParserCSV();
    $delimiter = $source_config['delimiter'] == 'TAB' ? "\t" : $source_config['delimiter'];
    $parser->setDelimiter($delimiter);

    $iterator = new ParserCSVIterator($fetcher_result->getFilePath());
    if (empty($source_config['no_headers'])) {
      // Get first line and use it for column names, convert them to lower case.
      $header = $this->parseHeader($parser, $iterator);
      if (!$header) {
        return;
      }
      $parser->setColumnNames($header);
    }

    // Determine section to parse, parse.
    $start = $state->pointer ? $state->pointer : $parser->lastLinePos();
    $limit = $source->importer->getLimit();
    $rows = $this->parseItems($parser, $iterator, $start, $limit);

    // Report progress.
    $state->total = filesize($fetcher_result->getFilePath());
    $state->pointer = $parser->lastLinePos();
    $progress = $parser->lastLinePos() ? $parser->lastLinePos() : $state->total;
    $state->progress($state->total, $progress);
    
    // Process rows for Beeld en Geluid media nodes
    foreach($rows as &$row) {
      $row['bvd_file']    = 'barendenvandorp://' . $row['naam videofile'];
      $row['bvd_source']  = 'Barend en Van Dorp';
      
      list($hours, $minutes, $seconds) = explode(':', $row['lengte video (mm:ss)']);
      $row['bvd_length']  = $hours * 3600 + $minutes * 60 + $seconds;
      
      // Parse tags
      $tags = explode(';', $row['onderwerpen']);
      foreach($tags as &$tag) {
        $tag = utf8_encode(trim($tag));
      }
      $row['bvd_tags'] = array_filter($tags, 'strlen');

      // Parse people
      $people = explode(';', $row['sprekers'])
              + explode(';', $row['uitvoerenden']);
      foreach($people as $key => &$person) {
        $person = utf8_encode(trim($person));
      }
      $row['bvd_people'] = array_filter($people, 'strlen');
      
      $row['beschrijving website'] = utf8_encode($row['beschrijving website']);
    }
    
    // Create a result object and return it.
    return new FeedsParserResult($rows, $source->feed_nid);
  }
}

/**
 * Enclosure element, can be part of the result array.
 */
class FeedsBVDEnclosure extends FeedsEnclosure {
  protected $type;

  /**
   * Constructor, requires MIME type.
   *
   * @param $value
   *   A path to a local file or a URL to a remote document.
   * @param $mimetype
   *   The mime type of the resource.
   */
  public function __construct($value) {
    parent::__construct($value);
    
    // Set (MIME) type
    $this->mime_type = 'video/barendenvandorp';
    $this->type = 'video';
  }
  
  public function getType() {
    return $this->type;
  }

  /**
   * Get a Drupal file object of the enclosed resource, download if necessary.
   *
   * @param $destination
   *   The path or uri specifying the target directory in which the file is
   *   expected. Don't use trailing slashes unless it's a streamwrapper scheme.
   *
   * @return
   *   A Drupal temporary file object of the enclosed resource.
   *
   * @throws Exception
   *   If file object could not be created.
   */
  public function getFile($destination) {
    $file = NULL;

    if ($this->getValue()) {
      $file = file_uri_to_object($this->getValue());
      // Override file MIME
      $file->filemime = $this->mime_type;
      // Append file entity type
      $file->type = $this->type;
      
      file_save($file);

      // We couldn't make sense of this enclosure, throw an exception.
      if (!$file) {
        throw new Exception(t('Invalid enclosure %enclosure', array('%enclosure' => $this->getValue())));
      }
    }

    return $file;
  }
}
