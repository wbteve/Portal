<?php
/**
 * @file
 * Class definition of FeedsFileProcessor.
 */

/**
 * Creates files from feed items.
 */
class FeedsFileProcessor extends FeedsProcessor {
  /**
   * Define entity type.
   */
  public function entityType() {
    return 'file';
  }

  /**
   * Implements parent::entityInfo().
   */
  protected function entityInfo() {
    $info = parent::entityInfo();
    $info['label plural'] = t('Files');
    return $info;
  }

  /**
   * Creates a new file in memory and returns it.
   */
  protected function newEntity(FeedsSource $source) {
    $file = new stdClass();
    $file->type = $this->config['file_type'];
    $file->timestamp = REQUEST_TIME;
    $file->log = 'Created by FeedsFileProcessor';
    $file->uid = $this->config['author'];
    return $file;
  }

  /**
   * Loads an existing file.
   *
   * If the update existing method is not FEEDS_UPDATE_EXISTING, only the managed file
   * table will be loaded, foregoing the file_load API for better performance.
   */
  protected function entityLoad(FeedsSource $source, $fid) {
    if ($this->config['update_existing'] == FEEDS_UPDATE_EXISTING) {
      $file = file_load($fid);
    }
    else {
      // We're replacing the existing file. Only save the absolutely necessary.
      $file = db_query("SELECT timestamp, fid, type FROM {file_managed} WHERE fid = :fid", array(':fid' => $fid))->fetch();
      $file->uid = $this->config['author'];
    }
    
    if ($this->config['update_existing'] == FEEDS_UPDATE_EXISTING) {
      $file->log = 'Updated by FeedsFileProcessor';
    }
    else {
      $file->log = 'Replaced by FeedsFileProcessor';
    }
    return $file;
  }

  /**
   * Save a file.
   */
  public function entitySave($entity) {
    file_save($entity);
  }

  /**
   * Delete a series of files.
   */
  protected function entityDeleteMultiple($fids) {
    foreach($fids as $fid) {
      $file = file_load($fid);
      file_delete($file);
    }
  }

  /**
   * Implement expire().
   *
   * @todo: move to processor stage?
   */
  public function expire($time = NULL) {
    if ($time === NULL) {
      $time = $this->expiryTime();
    }
    if ($time == FEEDS_EXPIRE_NEVER) {
      return;
    }
    $count = $this->getLimit();
    $files = db_query_range("SELECT f.fid FROM {file_managed} f JOIN {feeds_item} fi ON fi.entity_type = 'file' AND f.fid = fi.entity_id WHERE fi.id = :id AND f.timestamp < :created", 0, $count, array(':id' => $this->id, ':created' => REQUEST_TIME - $time));
    $fids = array();
    foreach ($files as $file) {
      $fids[$file->fid] = $file->fid;
    }
    $this->entityDeleteMultiple($fids);
    if (db_query_range("SELECT 1 FROM {file_managed} f JOIN {feeds_item} fi ON fi.entity_type = 'file' AND f.fid = fi.entity_id WHERE fi.id = :id AND n.created < :created", 0, 1, array(':id' => $this->id, ':created' => REQUEST_TIME - $time))->fetchField()) {
      return FEEDS_BATCH_ACTIVE;
    }
    return FEEDS_BATCH_COMPLETE;
  }

  /**
   * Return expiry time.
   */
  public function expiryTime() {
    return $this->config['expire'];
  }

  /**
   * Override parent::configDefaults().
   */
  public function configDefaults() {
    $types = media_type_get_types();
    $type = isset($types['default']) ? 'default' : key($types);
    return array(
      'file_type' => $type,
      'expire' => FEEDS_EXPIRE_NEVER,
      'author' => 0,
    ) + parent::configDefaults();
  }

  /**
   * Override parent::configForm().
   */
  public function configForm(&$form_state) {
    $types = media_type_get_types();
    foreach($types as &$type) {
      $type = $type->label;
    }
    array_walk($types, 'check_plain');
    
    $form = parent::configForm($form_state);
    $form['file_type'] = array(
      '#type' => 'select',
      '#title' => t('File type'),
      '#description' => t('Select the file type for the files to be created. <strong>Note:</strong> Users with "import !feed_id feeds" permissions will be able to <strong>import</strong> files of the content type selected here regardless of the file level permissions. Further, users with "clear !feed_id permissions" will be able to <strong>delete</strong> imported files regardless of their file level permissions.', array('!feed_id' => $this->id)),
      '#options' => $types,
      '#default_value' => $this->config['file_type'],
    );
    $author = user_load($this->config['author']);
    $form['author'] = array(
      '#type' => 'textfield',
      '#title' => t('Author'),
      '#description' => t('Select the author of the files to be created - leave empty to assign "anonymous".'),
      '#autocomplete_path' => 'user/autocomplete',
      '#default_value' => empty($author->name) ?  'anonymous' : check_plain($author->name),
    );
    $period = drupal_map_assoc(array(FEEDS_EXPIRE_NEVER, 3600, 10800, 21600, 43200, 86400, 259200, 604800, 604800 * 4, 604800 * 12, 604800 * 24, 31536000), 'feeds_format_expire');
    $form['expire'] = array(
      '#type' => 'select',
      '#title' => t('Expire files'),
      '#options' => $period,
      '#description' => t('Select after how much time files should be deleted. The file\'s published date will be used for determining the file\'s age, see Mapping settings.'),
      '#default_value' => $this->config['expire'],
    );
    $form['update_existing']['#options'] = array(
      FEEDS_SKIP_EXISTING => 'Do not update existing files',
      FEEDS_REPLACE_EXISTING => 'Replace existing files',
      FEEDS_UPDATE_EXISTING => 'Update existing files (slower than replacing them)',
    );
    return $form;
  }

  /**
   * Override parent::configFormValidate().
   */
  public function configFormValidate(&$values) {
    if ($author = user_load_by_name($values['author'])) {
      $values['author'] = $author->uid;
    }
    else {
      $values['author'] = 0;
    }
  }

  /**
   * Reschedule if expiry time changes.
   */
  public function configFormSubmit(&$values) {
    if ($this->config['expire'] != $values['expire']) {
      feeds_reschedule($this->id);
    }
    parent::configFormSubmit($values);
  }

  /**
   * Override setTargetElement to operate on a target item that is a file.
   */
  public function setTargetElement(FeedsSource $source, $target_file, $target_element, $value) {
    switch ($target_element) {
      case 'created':
        $target_file->timestamp = feeds_to_unixtime($value, REQUEST_TIME);
        break;
      case 'feeds_source':
        // Get the class of the feed file importer's fetcher and set the source
        // property. See feeds_file_update() how $file->feeds gets stored.
        if ($id = feeds_get_importer_id($this->config['file_type'])) {
          $class = get_class(feeds_importer($id)->fetcher);
          $target_file->feeds[$class]['source'] = $value;
          // This effectively suppresses 'import on submission' feature.
          // See feeds_file_insert().
          $target_file->feeds['suppress_import'] = TRUE;
        }
        break;
      default:
        parent::setTargetElement($source, $target_file, $target_element, $value);
        break;
    }
  }

  /**
   * Return available mapping targets.
   */
  public function getMappingTargets() {
    $types = media_type_get_types();
    $type = $types[$this->config['file_type']];
    $targets = parent::getMappingTargets();
    $targets += array(
      'fid' => array(
        'name' => t('File ID'),
        'description' => t('The fid of the file. NOTE: use this feature with care, file ids are usually assigned by Drupal.'),
        'optional_unique' => TRUE,
      ),
      'uid' => array(
        'name' => t('User ID'),
        'description' => t('The Drupal user ID of the file author.'),
      ),
      'filename' => array(
        'name' => t('Filename'),
        'description' => t('The name of the file.'),
      ),
      'uri' => array(
        'name' => t('File URI'),
        'description' => t('Location of the file.'),
        'optional_unique' => TRUE,
      ),
      'filemime' => array(
        'name' => t('MIME type'),
        'description' => t('The MIME type of the file.'),
      ),
      'status' => array(
        'name' => t('Published status'),
        'description' => t('Whether a file is published or not. 1 stands for published, 0 for not published.'),
      ),
      'timestamp' => array(
        'name' => t('Published date'),
        'description' => t('The UNIX time when a file has been published.'),
      ),
    );
    // If the target content type is a Feed file, expose its source field.
    if ($id = feeds_get_importer_id($this->config['file_type'])) {
      $name = feeds_importer($id)->config['name'];
      $targets['feeds_source'] = array(
        'name' => t('Feed source'),
        'description' => t('The content type created by this processor is a Feed File, it represents a source itself. Depending on the fetcher selected on the importer "@importer", this field is expected to be for example a URL or a path to a file.', array('@importer' => $name)),
        'optional_unique' => TRUE,
      );
    }

    // Let other modules expose mapping targets.
    self::loadMappers();
    feeds_alter('feeds_processor_targets', $targets, 'file', $this->config['file_type']);

    return $targets;
  }

  /**
   * Get nid of an existing feed item file if available.
   */
  protected function existingEntityId(FeedsSource $source, FeedsParserResult $result) {
    if ($fid = parent::existingEntityId($source, $result)) {
      return $fid;
    }

    // Iterate through all unique targets and test whether they do already
    // exist in the database.
    foreach ($this->uniqueTargets($source, $result) as $target => $value) {
      switch ($target) {
        case 'fid':
          $nid = db_query("SELECT nid FROM {node} WHERE nid = :nid", array(':nid' => $value))->fetchField();
          break;
        case 'uri':
          if ($id = feeds_get_importer_id($this->config['file_type'])) {
            $nid = db_query("SELECT fs.feed_nid FROM {node} n JOIN {feeds_source} fs ON n.nid = fs.feed_nid WHERE fs.id = :id AND fs.source = :source", array(':id' => $id, ':source' => $value))->fetchField();
          }
          break;
      }
      if ($fid) {
        // Return with the first nid found.
        return $fid;
      }
    }
    return 0;
  }
}
