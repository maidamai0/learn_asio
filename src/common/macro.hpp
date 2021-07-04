
/**
 * @brief disable copy constructor and assignment operator
 *
 */
#define no_copy(class_name)                    \
  class_name(const class_name& copy) = delete; \
  class_name& operator=(const class_name& copy) = delete

/**
 * @brief disable move constructor and move operator
 *
 */
#define no_move(class_name)               \
  class_name(class_name&& copy) = delete; \
  class_name& operator=(class_name&& copy) = delete
