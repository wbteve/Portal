<?php
/**
 * Class used by FeedsOAIOIParser
 * @author Robbert
 *
 */
class MediaInternetOIHandler extends MediaInternetBaseHandler {
  protected $uri;
  protected $mime_type;
  protected $type;

  public function __construct($embedCode) {
    parent::__construct($embedCode);
    // parse_url chockes on : in urls. Do it with substr
    // $path = parse_url($embedCode, PHP_URL_PATH);
    $path = substr($embedCode, strrpos($embedCode, '/'));

    if(!is_null($path)) {
      $this->uri = str_replace($path, '', $this->embedCode);

      // Set (MIME) type
      switch($path) {
        case '/Moving Image':
          $this->mime_type = 'video/openbeelden';
          $this->type = 'video';
          break;

        case '/Still Image':
        case '/Image':
          $this->mime_type = 'image/openbeelden';
          $this->type = 'image';
          break;

        case '/Sound Recording':
        case '/Sound':
          $this->mime_type = 'audio/openbeelden';
          $this->type = 'audio';
          break;
      }
    }
  }

  public function claim($embedCode) {
    // For some reason this doesn't seem to work
    // $scheme = parse_url($embedCode, PHP_URL_SCHEME);
    $scheme = substr($embedCode, 0, strpos($embedCode, ':'));
    return $scheme == 'openimages';
  }

  public function validate() {
    // @todo Media module currently fails when two files try to have the same
    //   URI, so catch that in the validation step. Some day, it would be nice
    //   to allow it, however. See http://drupal.org/node/952422.
    $existing_files = file_load_multiple(array(), array('uri' => $this->uri));
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
      $file = file_uri_to_object($this->uri);
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
