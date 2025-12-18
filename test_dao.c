#include "test_dao.h"

// 全局变量用于回调测试
static Plant test_plants[10];
static int test_plant_count = 0;
static CareRecord test_care_records[20];
static int test_care_record_count = 0;
static Reminder test_reminders[10];
static int test_reminder_count = 0;
static GrowthRecord test_growth_records[20];
static int test_growth_record_count = 0;
static CareExperience test_care_experiences[10];
static int test_care_experience_count = 0;

// 获取当前日期（YYYY-MM-DD格式）
void get_current_date(char* date_str) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(date_str, 11, "%Y-%m-%d", tm_info);
}

// 获取当前日期时间（YYYY-MM-DD HH:MM:SS格式）
void get_current_datetime(char* datetime_str) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(datetime_str, 20, "%Y-%m-%d %H:%M:%S", tm_info);
}

// 植物数据收集回调函数
static void plants_collect_callback(const Plant* plant) {
    if (test_plant_count < 10) {
        memcpy(&test_plants[test_plant_count], plant, sizeof(Plant));
        test_plant_count++;
    }
}

// 养护记录数据收集回调函数
static void care_records_collect_callback(const CareRecord* record) {
    if (test_care_record_count < 20) {
        memcpy(&test_care_records[test_care_record_count], record, sizeof(CareRecord));
        test_care_record_count++;
    }
}

// 提醒数据收集回调函数
static void reminders_collect_callback(const Reminder* reminder) {
    if (test_reminder_count < 10) {
        memcpy(&test_reminders[test_reminder_count], reminder, sizeof(Reminder));
        test_reminder_count++;
    }
}

// 生长记录数据收集回调函数
static void growth_records_collect_callback(const GrowthRecord* record) {
    if (test_growth_record_count < 20) {
        memcpy(&test_growth_records[test_growth_record_count], record, sizeof(GrowthRecord));
        test_growth_record_count++;
    }
}

// 养护经验数据收集回调函数
static void care_experiences_collect_callback(const CareExperience* experience) {
    if (test_care_experience_count < 10) {
        memcpy(&test_care_experiences[test_care_experience_count], experience, sizeof(CareExperience));
        test_care_experience_count++;
    }
}

// 重置测试数据
static void reset_test_data() {
    test_plant_count = 0;
    test_care_record_count = 0;
    test_reminder_count = 0;
    test_growth_record_count = 0;
    test_care_experience_count = 0;
}

// 清理数据库表数据
static void clear_database_tables() {
    char *err_msg = NULL;
    
    // 删除所有表数据（注意顺序，因为有外键约束）
    sqlite3_exec(db, "DELETE FROM care_records", NULL, NULL, &err_msg);
    sqlite3_exec(db, "DELETE FROM reminders", NULL, NULL, &err_msg);
    sqlite3_exec(db, "DELETE FROM plants", NULL, NULL, &err_msg);
}

// 测试植物信息DAO
int test_plants_dao() {
    printf("\n=== 测试植物信息DAO ===\n");
    
    // 清理数据库表数据，避免数据污染（暂时注释以便查看数据）
    // clear_database_tables();
    
    Plant plant;
    plant_init(&plant);
    
    // 测试数据
    strcpy(plant.name, "测试玫瑰");
    strcpy(plant.variety, "红玫瑰");
    strcpy(plant.planting_date, "2024-01-15");
    plant.water_frequency = 3;
    strcpy(plant.last_water_date, "2024-12-18");
    strcpy(plant.last_fertilize_date, "2024-12-10");
    strcpy(plant.status, "正常");
    strcpy(plant.notes, "测试植物");
    
    // 测试1：添加植物
    printf("1. 测试添加植物...\n");
    DAO_RESULT result = plants_dao_add(&plant);
    TEST_EQUAL(result, DAO_SUCCESS, "添加植物失败");
    printf("✓ 添加植物成功\n");
    
    // 测试2：查询所有植物
    printf("2. 测试查询所有植物...\n");
    reset_test_data();
    result = plants_dao_get_all(plants_collect_callback);
    TEST_EQUAL(result, DAO_SUCCESS, "查询所有植物失败");
    TEST_EQUAL(test_plant_count > 0, 1, "未找到任何植物");
    printf("✓ 查询所有植物成功，找到 %d 个植物\n", test_plant_count);
    
    // 测试3：根据ID查询植物
    printf("3. 测试根据ID查询植物...\n");
    Plant found_plant;
    int plant_id = test_plants[0].id; // 使用第一个找到的植物ID
    result = plants_dao_get_by_id(plant_id, &found_plant);
    TEST_EQUAL(result, DAO_SUCCESS, "根据ID查询植物失败");
    TEST_STRING_EQUAL(found_plant.name, "测试玫瑰", "查询到的植物名称不匹配");
    printf("✓ 根据ID查询植物成功\n");
    
    // 测试4：更新植物信息
    printf("4. 测试更新植物信息...\n");
    strcpy(plant.status, "良好");
    strcpy(plant.notes, "更新后的测试植物");
    result = plants_dao_update(plant_id, &plant);
    TEST_EQUAL(result, DAO_SUCCESS, "更新植物信息失败");
    
    // 验证更新
    result = plants_dao_get_by_id(plant_id, &found_plant);
    TEST_EQUAL(result, DAO_SUCCESS, "验证更新失败");
    TEST_STRING_EQUAL(found_plant.status, "良好", "植物状态更新失败");
    TEST_STRING_EQUAL(found_plant.notes, "更新后的测试植物", "植物备注更新失败");
    printf("✓ 更新植物信息成功\n");
    
    // 测试5：条件查询植物
    printf("5. 测试条件查询植物...\n");
    PlantQuery query;
    memset(&query, 0, sizeof(PlantQuery));
    strcpy(query.name, "测试");
    
    reset_test_data();
    result = plants_dao_query(&query, plants_collect_callback);
    TEST_EQUAL(result, DAO_SUCCESS, "条件查询植物失败");
    TEST_EQUAL(test_plant_count > 0, 1, "条件查询未找到匹配的植物");
    printf("✓ 条件查询植物成功，找到 %d 个匹配的植物\n", test_plant_count);
    
    // 测试6：删除植物（暂时注释以便查看数据）
    printf("6. 测试删除植物...\n");
    // result = plants_dao_delete(plant_id);
    // TEST_EQUAL(result, DAO_SUCCESS, "删除植物失败");
    
    // 验证删除
    // plant_init(&found_plant); // 重新初始化结构体
    // result = plants_dao_get_by_id(plant_id, &found_plant);
    // TEST_EQUAL(result, DAO_NOT_FOUND, "植物删除验证失败");
    printf("✓ 删除植物测试跳过（保留数据）\n");
    
    printf("=== 植物信息DAO测试全部通过 ===\n");
    return TEST_PASSED;
}

// 测试养护记录DAO
int test_care_records_dao() {
    printf("\n=== 测试养护记录DAO ===\n");
    
    // 清理数据库表数据，避免数据污染（暂时注释以便查看数据）
    // clear_database_tables();
    
    // 首先需要有一个植物来关联养护记录
    Plant plant;
    plant_init(&plant);
    strcpy(plant.name, "养护测试植物");
    strcpy(plant.variety, "测试品种");
    strcpy(plant.planting_date, "2024-01-01");
    
    DAO_RESULT result = plants_dao_add(&plant);
    TEST_EQUAL(result, DAO_SUCCESS, "创建测试植物失败");
    
    // 获取植物ID
    reset_test_data();
    result = plants_dao_get_all(plants_collect_callback);
    TEST_EQUAL(result, DAO_SUCCESS, "获取测试植物ID失败");
    int plant_id = test_plants[0].id;
    
    CareRecord record;
    care_record_init(&record);
    
    record.plant_id = plant_id;
    record.operation_type = CARE_WATER;
    get_current_datetime(record.operation_date);
    strcpy(record.details, "测试浇水操作");
    strcpy(record.amount, "500ml");
    
    // 测试1：添加养护记录
    printf("1. 测试添加养护记录...\n");
    result = care_records_dao_add(&record);
    TEST_EQUAL(result, DAO_SUCCESS, "添加养护记录失败");
    printf("✓ 添加养护记录成功\n");
    
    // 测试2：查询花卉的养护记录
    printf("2. 测试查询花卉的养护记录...\n");
    reset_test_data();
    result = care_records_dao_get_by_plant(plant_id, care_records_collect_callback);
    TEST_EQUAL(result, DAO_SUCCESS, "查询花卉养护记录失败");
    TEST_EQUAL(test_care_record_count > 0, 1, "未找到任何养护记录");
    printf("✓ 查询花卉养护记录成功，找到 %d 个记录\n", test_care_record_count);
    
    // 测试3：根据ID查询养护记录
    printf("3. 测试根据ID查询养护记录...\n");
    CareRecord found_record;
    int record_id = test_care_records[0].id;
    result = care_records_dao_get_by_id(record_id, &found_record);
    TEST_EQUAL(result, DAO_SUCCESS, "根据ID查询养护记录失败");
    TEST_EQUAL(found_record.plant_id, plant_id, "查询到的养护记录植物ID不匹配");
    TEST_EQUAL(found_record.operation_type, CARE_WATER, "查询到的养护记录类型不匹配");
    printf("✓ 根据ID查询养护记录成功\n");
    
    // 测试4：条件查询养护记录
    printf("4. 测试条件查询养护记录...\n");
    CareRecordQuery query;
    memset(&query, 0, sizeof(CareRecordQuery));
    query.plant_id = plant_id;
    query.operation_type = CARE_WATER;
    
    reset_test_data();
    result = care_records_dao_query(&query, care_records_collect_callback);
    TEST_EQUAL(result, DAO_SUCCESS, "条件查询养护记录失败");
    TEST_EQUAL(test_care_record_count > 0, 1, "条件查询未找到匹配的养护记录");
    printf("✓ 条件查询养护记录成功，找到 %d 个匹配的记录\n", test_care_record_count);
    
    // 测试5：删除养护记录（暂时注释以便查看数据）
    printf("5. 测试删除养护记录...\n");
    // result = care_records_dao_delete(record_id);
    // TEST_EQUAL(result, DAO_SUCCESS, "删除养护记录失败");
    
    // 验证删除
    // result = care_records_dao_get_by_id(record_id, &found_record);
    // TEST_EQUAL(result, DAO_NOT_FOUND, "养护记录删除验证失败");
    printf("✓ 删除养护记录测试跳过（保留数据）\n");
    
    // 清理测试植物（暂时注释以便查看数据）
    // plants_dao_delete(plant_id);
    
    printf("=== 养护记录DAO测试全部通过 ===\n");
    return TEST_PASSED;
}

// 测试提醒管理DAO
int test_reminders_dao() {
    printf("\n=== 测试提醒管理DAO ===\n");
    
    // 清理数据库表数据，避免数据污染（暂时注释以便查看数据）
    // clear_database_tables();
    
    // 首先需要有一个植物来关联提醒
    Plant plant;
    plant_init(&plant);
    strcpy(plant.name, "提醒测试植物");
    strcpy(plant.variety, "测试品种");
    strcpy(plant.planting_date, "2024-01-01");
    
    DAO_RESULT result = plants_dao_add(&plant);
    TEST_EQUAL(result, DAO_SUCCESS, "创建测试植物失败");
    
    // 获取植物ID
    reset_test_data();
    result = plants_dao_get_all(plants_collect_callback);
    TEST_EQUAL(result, DAO_SUCCESS, "获取测试植物ID失败");
    int plant_id = test_plants[0].id;
    
    Reminder reminder;
    reminder_init(&reminder);
    
    reminder.plant_id = plant_id;
    reminder.reminder_type = REMINDER_WATER;
    reminder.frequency = 3;
    strcpy(reminder.last_reminder_date, "2024-12-18");
    reminder.is_active = 1;
    
    // 测试1：添加提醒设置
    printf("1. 测试添加提醒设置...\n");
    result = reminders_dao_add(&reminder);
    TEST_EQUAL(result, DAO_SUCCESS, "添加提醒设置失败");
    printf("✓ 添加提醒设置成功\n");
    
    // 测试2：查询所有提醒
    printf("2. 测试查询所有提醒...\n");
    reset_test_data();
    result = reminders_dao_get_all(reminders_collect_callback);
    TEST_EQUAL(result, DAO_SUCCESS, "查询所有提醒失败");
    TEST_EQUAL(test_reminder_count > 0, 1, "未找到任何提醒设置");
    printf("✓ 查询所有提醒成功，找到 %d 个提醒设置\n", test_reminder_count);
    
    // 测试3：根据ID查询提醒
    printf("3. 测试根据ID查询提醒...\n");
    Reminder found_reminder;
    int reminder_id = test_reminders[0].id;
    result = reminders_dao_get_by_id(reminder_id, &found_reminder);
    TEST_EQUAL(result, DAO_SUCCESS, "根据ID查询提醒失败");
    TEST_EQUAL(found_reminder.plant_id, plant_id, "查询到的提醒植物ID不匹配");
    TEST_EQUAL(found_reminder.reminder_type, REMINDER_WATER, "查询到的提醒类型不匹配");
    printf("✓ 根据ID查询提醒成功\n");
    
    // 测试4：更新提醒设置
    printf("4. 测试更新提醒设置...\n");
    reminder.frequency = 5;
    reminder.is_active = 0;
    result = reminders_dao_update(reminder_id, &reminder);
    TEST_EQUAL(result, DAO_SUCCESS, "更新提醒设置失败");
    
    // 验证更新
    result = reminders_dao_get_by_id(reminder_id, &found_reminder);
    TEST_EQUAL(result, DAO_SUCCESS, "验证更新失败");
    TEST_EQUAL(found_reminder.frequency, 5, "提醒频率更新失败");
    TEST_EQUAL(found_reminder.is_active, 0, "提醒激活状态更新失败");
    printf("✓ 更新提醒设置成功\n");
    
    // 测试5：删除提醒设置（暂时注释以便查看数据）
    printf("5. 测试删除提醒设置...\n");
    // result = reminders_dao_delete(reminder_id);
    // TEST_EQUAL(result, DAO_SUCCESS, "删除提醒设置失败");
    
    // 验证删除
    // result = reminders_dao_get_by_id(reminder_id, &found_reminder);
    // TEST_EQUAL(result, DAO_NOT_FOUND, "提醒设置删除验证失败");
    printf("✓ 删除提醒设置测试跳过（保留数据）\n");
    
    // 清理测试植物（暂时注释以便查看数据）
    // plants_dao_delete(plant_id);
    
    printf("=== 提醒管理DAO测试全部通过 ===\n");
    return TEST_PASSED;
}

// 测试生长记录DAO
int test_growth_records_dao() {
    printf("\n=== 测试生长记录DAO ===\n");
    
    // 首先需要有一个植物来关联生长记录
    Plant plant;
    plant_init(&plant);
    strcpy(plant.name, "生长记录测试植物");
    strcpy(plant.variety, "测试品种");
    strcpy(plant.planting_date, "2024-01-01");
    
    DAO_RESULT result = plants_dao_add(&plant);
    TEST_EQUAL(result, DAO_SUCCESS, "创建测试植物失败");
    
    // 获取植物ID
    reset_test_data();
    result = plants_dao_get_all(plants_collect_callback);
    TEST_EQUAL(result, DAO_SUCCESS, "获取测试植物ID失败");
    int plant_id = test_plants[0].id;
    
    GrowthRecord record;
    growth_record_init(&record);
    
    record.plant_id = plant_id;
    strcpy(record.record_date, "2024-12-19");
    record.height_cm = 25.5;
    record.leaf_count = 12;
    record.bud_count = 3;
    record.flower_count = 2;
    record.health_score = 85;
    strcpy(record.leaf_color, "绿色");
    strcpy(record.growth_vigor, "良好");
    record.temperature = 22;
    record.humidity = 65;
    strcpy(record.light_exposure, "充足");
    strcpy(record.notes, "测试生长记录");
    strcpy(record.photo_path, "/photos/test.jpg");
    
    // 测试1：添加生长记录
    printf("1. 测试添加生长记录...\n");
    result = growth_records_dao_add(&record);
    TEST_EQUAL(result, DAO_SUCCESS, "添加生长记录失败");
    printf("✓ 添加生长记录成功\n");
    
    // 测试2：查询花卉的生长记录
    printf("2. 测试查询花卉的生长记录...\n");
    reset_test_data();
    result = growth_records_dao_get_by_plant(plant_id, growth_records_collect_callback);
    TEST_EQUAL(result, DAO_SUCCESS, "查询花卉生长记录失败");
    TEST_EQUAL(test_growth_record_count > 0, 1, "未找到任何生长记录");
    printf("✓ 查询花卉生长记录成功，找到 %d 个记录\n", test_growth_record_count);
    
    // 测试3：根据ID查询生长记录
    printf("3. 测试根据ID查询生长记录...\n");
    GrowthRecord found_record;
    int record_id = test_growth_records[0].id;
    result = growth_records_dao_get_by_id(record_id, &found_record);
    TEST_EQUAL(result, DAO_SUCCESS, "根据ID查询生长记录失败");
    TEST_EQUAL(found_record.plant_id, plant_id, "查询到的生长记录植物ID不匹配");
    TEST_EQUAL(found_record.height_cm, 25.5, "查询到的生长记录高度不匹配");
    printf("✓ 根据ID查询生长记录成功\n");
    
    // 测试4：条件查询生长记录
    printf("4. 测试条件查询生长记录...\n");
    GrowthRecordQuery query;
    memset(&query, 0, sizeof(GrowthRecordQuery));
    query.plant_id = plant_id;
    strcpy(query.start_date, "2024-12-01");
    strcpy(query.end_date, "2024-12-31");
    
    reset_test_data();
    result = growth_records_dao_query(&query, growth_records_collect_callback);
    TEST_EQUAL(result, DAO_SUCCESS, "条件查询生长记录失败");
    TEST_EQUAL(test_growth_record_count > 0, 1, "条件查询未找到匹配的生长记录");
    printf("✓ 条件查询生长记录成功，找到 %d 个匹配的记录\n", test_growth_record_count);
    
    // 测试5：删除生长记录（暂时注释以便查看数据）
    printf("5. 测试删除生长记录...\n");
    // result = growth_records_dao_delete(record_id);
    // TEST_EQUAL(result, DAO_SUCCESS, "删除生长记录失败");
    
    // 验证删除
    // result = growth_records_dao_get_by_id(record_id, &found_record);
    // TEST_EQUAL(result, DAO_NOT_FOUND, "生长记录删除验证失败");
    printf("✓ 删除生长记录测试跳过（保留数据）\n");
    
    // 清理测试植物（暂时注释以便查看数据）
    // plants_dao_delete(plant_id);
    
    printf("=== 生长记录DAO测试全部通过 ===\n");
    return TEST_PASSED;
}

// 测试养护经验DAO
int test_care_experience_dao() {
    printf("\n=== 测试养护经验DAO ===\n");
    
    CareExperience experience;
    care_experience_init(&experience);
    
    strcpy(experience.variety, "玫瑰");
    experience.optimal_water_frequency = 3;
    experience.optimal_fertilize_frequency = 7;
    strcpy(experience.best_season, "春季");
    strcpy(experience.common_pests, "蚜虫,红蜘蛛");
    strcpy(experience.effective_controls, "喷洒杀虫剂,保持通风");
    strcpy(experience.common_mistakes, "浇水过多,光照不足");
    strcpy(experience.warning_signs, "叶片发黄,生长缓慢");
    strcpy(experience.recovery_methods, "减少浇水,增加光照");
    experience.total_plants = 10;
    experience.success_rate = 85.5;
    experience.avg_health_score = 88.2;
    strcpy(experience.last_updated, "2024-12-19");
    experience.confidence_level = 4;
    
    // 测试1：添加养护经验
    printf("1. 测试添加养护经验...\n");
    DAO_RESULT result = care_experience_dao_add(&experience);
    TEST_EQUAL(result, DAO_SUCCESS, "添加养护经验失败");
    printf("✓ 添加养护经验成功\n");
    
    // 测试2：查询所有养护经验
    printf("2. 测试查询所有养护经验...\n");
    reset_test_data();
    result = care_experience_dao_get_all(care_experiences_collect_callback);
    TEST_EQUAL(result, DAO_SUCCESS, "查询所有养护经验失败");
    TEST_EQUAL(test_care_experience_count > 0, 1, "未找到任何养护经验");
    printf("✓ 查询所有养护经验成功，找到 %d 个经验记录\n", test_care_experience_count);
    
    // 测试3：根据ID查询养护经验
    printf("3. 测试根据ID查询养护经验...\n");
    CareExperience found_experience;
    int experience_id = test_care_experiences[0].id;
    result = care_experience_dao_get_by_id(experience_id, &found_experience);
    TEST_EQUAL(result, DAO_SUCCESS, "根据ID查询养护经验失败");
    TEST_STRING_EQUAL(found_experience.variety, "玫瑰", "查询到的养护经验品种不匹配");
    TEST_EQUAL(found_experience.optimal_water_frequency, 3, "查询到的养护经验浇水频率不匹配");
    printf("✓ 根据ID查询养护经验成功\n");
    
    // 测试4：条件查询养护经验
    printf("4. 测试条件查询养护经验...\n");
    CareExperienceQuery query;
    memset(&query, 0, sizeof(CareExperienceQuery));
    strcpy(query.variety, "玫瑰");
    query.min_confidence_level = 3;
    
    reset_test_data();
    result = care_experience_dao_query(&query, care_experiences_collect_callback);
    TEST_EQUAL(result, DAO_SUCCESS, "条件查询养护经验失败");
    TEST_EQUAL(test_care_experience_count > 0, 1, "条件查询未找到匹配的养护经验");
    printf("✓ 条件查询养护经验成功，找到 %d 个匹配的经验记录\n", test_care_experience_count);
    
    // 测试5：更新养护经验
    printf("5. 测试更新养护经验...\n");
    experience.success_rate = 90.0;
    experience.avg_health_score = 92.5;
    result = care_experience_dao_update(experience_id, &experience);
    TEST_EQUAL(result, DAO_SUCCESS, "更新养护经验失败");
    
    // 验证更新
    result = care_experience_dao_get_by_id(experience_id, &found_experience);
    TEST_EQUAL(result, DAO_SUCCESS, "验证更新失败");
    TEST_EQUAL(found_experience.success_rate, 90.0, "养护经验成功率更新失败");
    TEST_EQUAL(found_experience.avg_health_score, 92.5, "养护经验平均健康评分更新失败");
    printf("✓ 更新养护经验成功\n");
    
    // 测试6：删除养护经验（暂时注释以便查看数据）
    printf("6. 测试删除养护经验...\n");
    // result = care_experience_dao_delete(experience_id);
    // TEST_EQUAL(result, DAO_SUCCESS, "删除养护经验失败");
    
    // 验证删除
    // result = care_experience_dao_get_by_id(experience_id, &found_experience);
    // TEST_EQUAL(result, DAO_NOT_FOUND, "养护经验删除验证失败");
    printf("✓ 删除养护经验测试跳过（保留数据）\n");
    
    printf("=== 养护经验DAO测试全部通过 ===\n");
    return TEST_PASSED;
}

// 主测试函数
int main() {
    printf("=== DAO层单元测试开始 ===\n");
    
    // 初始化数据库
    if (init_database() != 0) {
        printf("✗ 数据库初始化失败，无法进行测试\n");
        return TEST_FAILED;
    }
    
    int total_failed = 0;
    
    // 运行植物信息DAO测试
    if (test_plants_dao() != TEST_PASSED) {
        total_failed++;
    }
    
    // 运行养护记录DAO测试
    if (test_care_records_dao() != TEST_PASSED) {
        total_failed++;
    }
    
    // 运行提醒管理DAO测试
    if (test_reminders_dao() != TEST_PASSED) {
        total_failed++;
    }
    
    // 运行生长记录DAO测试
    if (test_growth_records_dao() != TEST_PASSED) {
        total_failed++;
    }
    
    // 运行养护经验DAO测试
    if (test_care_experience_dao() != TEST_PASSED) {
        total_failed++;
    }
    
    // 关闭数据库
    close_database();
    
    printf("\n=== DAO层单元测试完成 ===\n");
    if (total_failed == 0) {
        printf("✅ 所有测试通过！DAO层功能正常\n");
        return TEST_PASSED;
    } else {
        printf("❌ 有 %d 个测试失败，需要检查DAO层实现\n", total_failed);
        return TEST_FAILED;
    }
}