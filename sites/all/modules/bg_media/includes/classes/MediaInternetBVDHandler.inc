<?php
/**
 * Class used by FeedsBVDCVSParser
 * @author Robbert
 *
 */
class MediaInternetBVDHandler extends MediaInternetBaseHandler {
  protected $mime_type;
  protected $type;

  public function __construct($embedCode) {
    parent::__construct($embedCode);

    // Set (MIME) type
    $this->mime_type = 'video/barendenvandorp';
    $this->type = 'video';
  }

  public function claim($embedCode) {
    $scheme = parse_url($embedCode, PHP_URL_SCHEME);
    return $scheme == 'barendenvandorp';
  }

  public function validate() {
    // @todo Media module currently fails when two files try to have the same
    //   URI, so catch that in the validation step. Some day, it would be nice
    //   to allow it, however. See http://drupal.org/node/952422.
    $existing_files = file_load_multiple(array(), array('uri' => $this->embedCode));
    if (count($existing_files)) {
      throw new MediaInternetValidationException(t('You have entered a URL for a video that is already in your library.'));
    }
  }

  public function save() {
    $file = $this->getFileObject();
    file_save($file);
    return $file;
  }

  public function getFileObject() {
    $file = NULL;

    if ($this->embedCode && !is_null($this->type)) {
      $file = file_uri_to_object($this->embedCode);
      // Override file MIME
      $file->filemime = $this->mime_type;
      // Append file entity type
      $file->type = $this->type;

      // We couldn't make sense of this enclosure, throw an exception.
      if (!$file) {
        throw new Exception(t('Invalid enclosure %enclosure', array('%enclosure' => $this->embedCode)));
      }
    }

    return $file;
  }
}