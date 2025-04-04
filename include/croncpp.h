#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <string_view>
#include <bitset>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <algorithm>

namespace cron
{
   using cron_int  = uint8_t;

   constexpr std::time_t INVALID_TIME = static_cast<std::time_t>(-1);

   constexpr size_t INVALID_INDEX = static_cast<size_t>(-1);

   class cronexpr;   

   namespace detail
   {
      enum class cron_field
      {
         second,
         minute,
         hour_of_day,
         day_of_week,
         day_of_month,
         month,
         year
      };

      template <typename Traits>
      static bool find_next(cronexpr const & cex,
                            std::tm& date,
                            size_t const dot);

      template <cron_field field>
      constexpr auto& cron_field_ref(cronexpr& cex);
   }

   struct bad_cronexpr : public std::runtime_error
   {
   public:
      explicit bad_cronexpr(std::string_view message) :
         std::runtime_error(message.data())
      {}
   };


   struct cron_standard_traits
   {
      static const cron_int CRON_MIN_SECONDS = 0;
      static const cron_int CRON_MAX_SECONDS = 59;

      static const cron_int CRON_MIN_MINUTES = 0;
      static const cron_int CRON_MAX_MINUTES = 59;

      static const cron_int CRON_MIN_HOURS = 0;
      static const cron_int CRON_MAX_HOURS = 23;

      static const cron_int CRON_MIN_DAYS_OF_WEEK = 0;
      static const cron_int CRON_MAX_DAYS_OF_WEEK = 6;

      static const cron_int CRON_MIN_DAYS_OF_MONTH = 1;
      static const cron_int CRON_MAX_DAYS_OF_MONTH = 31;

      static const cron_int CRON_MIN_MONTHS = 1;
      static const cron_int CRON_MAX_MONTHS = 12;

	  static const cron_int CRON_MIN_YEARS = 2019;
	  static const cron_int CRON_MAX_YEARS = 2028;

      static const cron_int CRON_MAX_YEARS_DIFF = 4;

      static const inline std::vector<std::string> DAYS = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
      static const inline std::vector<std::string> MONTHS = { "NIL", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
	  static const inline std::vector<std::string> YEARS = { "NIL", "二零一九","二零二零", "二零二一", "二零二二", "二零二三", "二零二四", "二零二五", "二零二六", "二零二七", "二零二八"};
   };

   struct cron_oracle_traits
   {
      static const cron_int CRON_MIN_SECONDS = 0;
      static const cron_int CRON_MAX_SECONDS = 59;

      static const cron_int CRON_MIN_MINUTES = 0;
      static const cron_int CRON_MAX_MINUTES = 59;

      static const cron_int CRON_MIN_HOURS = 0;
      static const cron_int CRON_MAX_HOURS = 23;

      static const cron_int CRON_MIN_DAYS_OF_WEEK = 1;
      static const cron_int CRON_MAX_DAYS_OF_WEEK = 7;

      static const cron_int CRON_MIN_DAYS_OF_MONTH = 1;
      static const cron_int CRON_MAX_DAYS_OF_MONTH = 31;

      static const cron_int CRON_MIN_MONTHS = 0;
      static const cron_int CRON_MAX_MONTHS = 11;

      static const cron_int CRON_MAX_YEARS_DIFF = 4;

      static const inline std::vector<std::string> DAYS = { "NIL", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
      static const inline std::vector<std::string> MONTHS = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
   };

   struct cron_quartz_traits
   {
      static const cron_int CRON_MIN_SECONDS = 0;
      static const cron_int CRON_MAX_SECONDS = 59;

      static const cron_int CRON_MIN_MINUTES = 0;
      static const cron_int CRON_MAX_MINUTES = 59;

      static const cron_int CRON_MIN_HOURS = 0;
      static const cron_int CRON_MAX_HOURS = 23;

      static const cron_int CRON_MIN_DAYS_OF_WEEK = 1;
      static const cron_int CRON_MAX_DAYS_OF_WEEK = 7;

      static const cron_int CRON_MIN_DAYS_OF_MONTH = 1;
      static const cron_int CRON_MAX_DAYS_OF_MONTH = 31;

      static const cron_int CRON_MIN_MONTHS = 1;
      static const cron_int CRON_MAX_MONTHS = 12;

      static const cron_int CRON_MAX_YEARS_DIFF = 4;

      static const inline std::vector<std::string> DAYS = { "NIL", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT" };
      static const inline std::vector<std::string> MONTHS = { "NIL", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
   };

   class cronexpr
   {
   public:
	  std::bitset<60> seconds;
      std::bitset<60> minutes;
      std::bitset<24> hours;
      std::bitset<7>  days_of_week;
      std::bitset<31> days_of_month;
      std::bitset<12> months;
	  std::bitset<10> years;

      friend bool operator==(cronexpr const & e1, cronexpr const & e2);
      friend bool operator!=(cronexpr const & e1, cronexpr const & e2);

      template <detail::cron_field field>
      friend constexpr auto& detail::cron_field_ref(cronexpr& cex);

      template <typename Traits>
      friend bool detail::find_next(cronexpr const & cex,
                                    std::tm& date,
                                    size_t const dot);

      friend std::string to_string(cronexpr const & cex);
   };

   inline bool operator==(cronexpr const & e1, cronexpr const & e2)
   {
      return
         e1.seconds == e2.seconds &&
         e1.minutes == e2.minutes &&
         e1.hours == e2.hours &&
         e1.days_of_week == e2.days_of_week &&
         e1.days_of_month == e2.days_of_month &&
         e1.months == e2.months;
   }

   inline bool operator!=(cronexpr const & e1, cronexpr const & e2)
   {
      return !(e1 == e2);
   }

   inline std::string to_string(cronexpr const & cex)
   {
      return
         cex.seconds.to_string() + " " +
         cex.minutes.to_string() + " " +
         cex.hours.to_string() + " " +
         cex.days_of_week.to_string() + " " +
         cex.days_of_month.to_string() + " " +
         cex.months.to_string();
   }

   namespace utils
   {
      inline std::time_t tm_to_time(std::tm& date)
      {
         return std::mktime(&date);
      }

      inline std::tm* time_to_tm(std::time_t const * date, std::tm* const out)
      {
#ifdef _WIN32
         errno_t err = localtime_s(out, date);
         return 0 == err ? out : nullptr;
#else
         return localtime_r(date, out);
#endif   
      }

      inline std::tm to_tm(std::string_view time)
      {
         std::istringstream str(time.data());
         str.imbue(std::locale(setlocale(LC_ALL, nullptr)));

         std::tm result{0};
         str >> std::get_time(&result, "%Y-%m-%d %H:%M:%S");
         if (str.fail()) throw std::runtime_error("Parsing date failed!");

         result.tm_isdst = -1; // DST info not available

         return result;
      }

      inline std::string to_string(std::tm const & tm)
      {
         std::ostringstream str;
         str.imbue(std::locale(setlocale(LC_ALL, nullptr)));
         str << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
         if (str.fail()) throw std::runtime_error("Writing date failed!");

         return str.str();
      }

      inline std::string to_upper(std::string text)
      {
         std::transform(std::begin(text), std::end(text),
            std::begin(text), static_cast<int(*)(int)>(std::toupper));

         return text;
      }

      static std::vector<std::string> split(std::string_view text, char const delimiter)
      {
         std::vector<std::string> tokens;
         std::string token;
         std::istringstream tokenStream(text.data());
         while (std::getline(tokenStream, token, delimiter))
         {
            tokens.push_back(token);
         }
         return tokens;
      }

      constexpr inline bool contains(std::string_view text, char const ch) noexcept
      {
         return std::string_view::npos != text.find_first_of(ch);
      }
   }

   namespace detail
   {

      inline cron_int to_cron_int(std::string_view text)
      {
         try
         {
            return static_cast<cron_int>(std::stoul(text.data()));
         }
         catch (std::exception const & ex)
         {
            throw bad_cronexpr(ex.what());
         }
      }

      static std::string replace_ordinals(
         std::string text, 
         std::vector<std::string> const & replacement)
      {
         for (size_t i = 0; i < replacement.size(); ++i)
         {
            auto pos = text.find(replacement[i]);
            if (std::string::npos != pos)
               text.replace(pos, 3 ,std::to_string(i));
         }

         return text;
      }

      static std::pair<cron_int, cron_int> make_range(
         std::string_view field,
         cron_int const minval,
         cron_int const maxval)
      {
         cron_int first = 0;
         cron_int last = 0;
         if (field.size() == 1 && field[0] == '*')
         {
            first = minval;
            last = maxval;
         } 
         else if (!utils::contains(field, '-'))
         {
            first = to_cron_int(field);
            last = first;
         }
         else
         {
            auto parts = utils::split(field, '-');
            if (parts.size() != 2)
               throw bad_cronexpr("Specified range requires two fields");

            first = to_cron_int(parts[0]);
            last = to_cron_int(parts[1]);
         }

         if (first > maxval || last > maxval)
         {
            throw bad_cronexpr("Specified range exceeds maximum");
         }
         if (first < minval || last < minval)
         {
            throw bad_cronexpr("Specified range is less than minimum");
         }
         if (first > last)
         {
            throw bad_cronexpr("Specified range start exceeds range end");
         }

         return { first, last };
      }

      template <size_t N>
      static void set_cron_field(
         std::string_view value,
         std::bitset<N>& target,
         cron_int const minval,
         cron_int const maxval)
      {
         if(value.length() > 0 && value[value.length()-1] == ',')
            throw bad_cronexpr("Value cannot end with comma");

         auto fields = utils::split(value, ',');
         if (fields.empty())
            throw bad_cronexpr("Expression parsing error");

         for (auto const & field : fields)
         {
            if (!utils::contains(field, '/'))
            {
               auto[first, last] = detail::make_range(field, minval, maxval);
               for (cron_int i = first - minval; i <= last - minval; ++i)
               {
                  target.set(i);
               }
            }
            else 
            {
               auto parts = utils::split(field, '/');
               if (parts.size() != 2)
                  throw bad_cronexpr("Incrementer must have two fields");

               auto[first, last] = detail::make_range(parts[0], minval, maxval);

               if (!utils::contains(parts[0], '-'))
               {
                  last = maxval;
               }

               auto delta = detail::to_cron_int(parts[1]);
               if(delta <= 0)
                  throw bad_cronexpr("Incrementer must be a positive value");

               for (cron_int i = first - minval; i <= last - minval; i += delta)
               {
                  target.set(i);
               }
            }
         }
		
		
      }

      template <typename Traits>
      static void set_cron_days_of_week(
         std::string value,
         std::bitset<7>& target)
      {
         auto days = utils::to_upper(value);
         auto days_replaced = detail::replace_ordinals(days, Traits::DAYS);

         if (days_replaced.size() == 1 && days_replaced[0] == '?')
            days_replaced[0] = '*';

         set_cron_field(
            days_replaced, 
            target, 
            Traits::CRON_MIN_DAYS_OF_WEEK,
            Traits::CRON_MAX_DAYS_OF_WEEK);         
      }

      template <typename Traits>
      static void set_cron_days_of_month(
         std::string value,
         std::bitset<31>& target)
      {
         if (value.size() == 1 && value[0] == '?')
            value[0] = '*';

         set_cron_field(
            value, 
            target, 
            Traits::CRON_MIN_DAYS_OF_MONTH,
            Traits::CRON_MAX_DAYS_OF_MONTH);
      }

      template <typename Traits>
      static void set_cron_month(
         std::string value,
         std::bitset<12>& target)
      {
         auto month = utils::to_upper(value);
         auto month_replaced = replace_ordinals(month, Traits::MONTHS);

         set_cron_field(
            month_replaced, 
            target, 
            Traits::CRON_MIN_MONTHS,
            Traits::CRON_MAX_MONTHS);
      }
	  template <typename Traits>
	  static void set_cron_year(
		  std::string value,
		  std::bitset<10>& target)
	  {
		  auto year = utils::to_upper(value);
		  auto year_replaced = replace_ordinals(year, Traits::YEARS);

		  set_cron_field(
			  year_replaced,
			  target,
			  Traits::CRON_MIN_YEARS,
			  Traits::CRON_MAX_YEARS);
	  }

      template <size_t N>
      inline size_t next_set_bit(
         std::bitset<N> const & target,
         size_t minimum,
         size_t maximum,
         size_t offset)
      {
         for (auto i = offset; i < N; ++i)
         {
            if (target.test(i)) return i;
         }

         return INVALID_INDEX;
      }

      inline void add_to_field(
         std::tm& date, 
         cron_field const field, 
         int const val)
      {
         switch (field)
         {
         case cron_field::second:
            date.tm_sec += val;
            break;
         case cron_field::minute:
            date.tm_min += val;
            break;
         case cron_field::hour_of_day:
            date.tm_hour += val;
            break;
         case cron_field::day_of_week:
         case cron_field::day_of_month:
            date.tm_mday += val;
            break;
         case cron_field::month:
            date.tm_mon += val;
            break;
         case cron_field::year:
            date.tm_year += val;
            break;
         }

         if (INVALID_TIME == utils::tm_to_time(date))
            throw bad_cronexpr("Invalid time expression");
      }

      inline void set_field(
         std::tm& date, 
         cron_field const field, 
         int const val)
      {
         switch (field)
         {
         case cron_field::second:
            date.tm_sec = val;
            break;
         case cron_field::minute:
            date.tm_min = val;
            break;
         case cron_field::hour_of_day:
            date.tm_hour = val;
            break;
         case cron_field::day_of_week:
            date.tm_wday = val;
            break;
         case cron_field::day_of_month:
            date.tm_mday = val;
            break;
         case cron_field::month:
            date.tm_mon = val;
            break;
         case cron_field::year:
            date.tm_year = val;
            break;
         }

         if (INVALID_TIME == utils::tm_to_time(date))
            throw bad_cronexpr("Invalid time expression");
      }

      inline void reset_field(
         std::tm& date, 
         cron_field const field)
      {
         switch (field)
         {
         case cron_field::second:
            date.tm_sec = 0;
            break;
         case cron_field::minute:
            date.tm_min = 0;
            break;
         case cron_field::hour_of_day:
            date.tm_hour = 0;
            break;
         case cron_field::day_of_week:
            date.tm_wday = 0;
            break;
         case cron_field::day_of_month:
            date.tm_mday = 1;
            break;
         case cron_field::month:
            date.tm_mon = 0;
            break;
         case cron_field::year:
            date.tm_year = 0;
            break;
         }

         if (INVALID_TIME == utils::tm_to_time(date))
            throw bad_cronexpr("Invalid time expression");
      }

      inline void reset_all_fields(
         std::tm& date, 
         std::bitset<7> const & marked_fields)
      {
         for (size_t i = 0; i < marked_fields.size(); ++i)
         {
            if (marked_fields.test(i))
               reset_field(date, static_cast<cron_field>(i));
         }
      }

      inline void mark_field(
         std::bitset<7> & orders, 
         cron_field const field)
      {
         if (!orders.test(static_cast<size_t>(field)))
            orders.set(static_cast<size_t>(field));
      }

      template <size_t N>
      static size_t find_next(
         std::bitset<N> const & target,
         std::tm& date,
         unsigned int const minimum,
         unsigned int const maximum, 
         unsigned int const value,
         cron_field const field,
         cron_field const next_field,
         std::bitset<7> const & marked_fields)
      {
         auto next_value = next_set_bit(target, minimum, maximum, value);
         if (INVALID_INDEX == next_value)
         {
            add_to_field(date, next_field, 1);
            reset_field(date, field);
            next_value = next_set_bit(target, minimum, maximum, 0);
         }

         if (INVALID_INDEX == next_value || next_value != value)
         {
            set_field(date, field, static_cast<int>(next_value));
            reset_all_fields(date, marked_fields);
         }

         return next_value;
      }
      
      template <typename Traits>
      static size_t find_next_day(
         std::tm& date,
         std::bitset<31> const & days_of_month,
         size_t day_of_month,
         std::bitset<7> const & days_of_week,
         size_t day_of_week,
         std::bitset<7> const & marked_fields)
      {
         unsigned int count = 0;
         unsigned int maximum = 366;
         while (
            (!days_of_month.test(day_of_month - Traits::CRON_MIN_DAYS_OF_MONTH) ||
            !days_of_week.test(day_of_week - Traits::CRON_MIN_DAYS_OF_WEEK)) 
            && count++ < maximum)
         {
            add_to_field(date, cron_field::day_of_month, 1);

            day_of_month = date.tm_mday;
            day_of_week = date.tm_wday;

            reset_all_fields(date, marked_fields);
         }

         return day_of_month;
      }

      template <typename Traits>
      static bool find_next(cronexpr const & cex,
                            std::tm& date,
                            size_t const dot)
      {
         bool res = true;

         std::bitset<7> marked_fields{ 0 };
         std::bitset<7> empty_list{ 0 };

         unsigned int second = date.tm_sec;
         auto updated_second = find_next(
            cex.seconds,
            date,
            Traits::CRON_MIN_SECONDS,
            Traits::CRON_MAX_SECONDS,
            second, 
            cron_field::second,
            cron_field::minute,
            empty_list);

         if (second == updated_second)
         {
            mark_field(marked_fields, cron_field::second);
         }

         unsigned int minute = date.tm_min;
         auto update_minute = find_next(
            cex.minutes,
            date,
            Traits::CRON_MIN_MINUTES,
            Traits::CRON_MAX_MINUTES,
            minute,
            cron_field::minute,
            cron_field::hour_of_day,
            marked_fields);
         if (minute == update_minute)
         {
            mark_field(marked_fields, cron_field::minute);
         }
         else
         {
            res = find_next<Traits>(cex, date, dot);
            if (!res) return res;
         }

         unsigned int hour = date.tm_hour;
         auto updated_hour = find_next(
            cex.hours,
            date,
            Traits::CRON_MIN_HOURS,
            Traits::CRON_MAX_HOURS,
            hour,
            cron_field::hour_of_day,
            cron_field::day_of_week,
            marked_fields);
         if (hour == updated_hour)
         {
            mark_field(marked_fields, cron_field::hour_of_day);
         }
         else
         {
            res = find_next<Traits>(cex, date, dot);
            if (!res) return res;
         }

         unsigned int day_of_week = date.tm_wday;
         unsigned int day_of_month = date.tm_mday;
         auto updated_day_of_month = find_next_day<Traits>(
            date,
            cex.days_of_month,
            day_of_month,
            cex.days_of_week,
            day_of_week,
            marked_fields);
         if (day_of_month == updated_day_of_month)
         {
            mark_field(marked_fields, cron_field::day_of_month);
         }
         else
         {
            res = find_next<Traits>(cex, date, dot);
            if (!res) return res;
         }

         unsigned int month = date.tm_mon;
         auto updated_month = find_next(
            cex.months,
            date,
            Traits::CRON_MIN_MONTHS,
            Traits::CRON_MAX_MONTHS,
            month,
            cron_field::month,
            cron_field::year,
            marked_fields);
         if (month != updated_month)
         {
            if (date.tm_year - dot > Traits::CRON_MAX_YEARS_DIFF)
               return false;

            res = find_next<Traits>(cex, date, dot);
            if (!res) return res;
         }

         return res;
      }      

      template <cron_field field>
      constexpr auto& cron_field_ref(cronexpr& cex)
      {
         if constexpr (field == cron_field::day_of_month) 
            return cex.days_of_month;
         else if constexpr (field == cron_field::day_of_week)
            return cex.days_of_week;
         else if constexpr (field == cron_field::hour_of_day)
            return cex.hours;
         else if constexpr (field == cron_field::minute)
            return cex.minutes;
         else if constexpr (field == cron_field::second)
            return cex.seconds;
         else if constexpr (field == cron_field::month)
            return cex.months;
		 else if constexpr (field == cron_field::year)
			 return cex.years;
      }
   }

   template <typename Traits = cron_standard_traits>
   static cronexpr make_cron(std::string_view expr)
   {
      cronexpr cex;

      if (expr.empty())
         throw bad_cronexpr("Invalid empty cron expression");

      auto fields = utils::split(expr, ' ');
      fields.erase(
         std::remove_if(std::begin(fields), std::end(fields),
            [](std::string_view s) {return s.empty(); }),
         std::end(fields));
      if (fields.size() != 7)
         throw bad_cronexpr("cron expression must have six fields");

      detail::set_cron_field(fields[0], detail::cron_field_ref<detail::cron_field::second>(cex), Traits::CRON_MIN_SECONDS, Traits::CRON_MAX_SECONDS);
      detail::set_cron_field(fields[1], detail::cron_field_ref<detail::cron_field::minute>(cex), Traits::CRON_MIN_MINUTES, Traits::CRON_MAX_MINUTES);
      detail::set_cron_field(fields[2], detail::cron_field_ref<detail::cron_field::hour_of_day>(cex), Traits::CRON_MIN_HOURS, Traits::CRON_MAX_HOURS);

      detail::set_cron_days_of_week<Traits>(fields[5], detail::cron_field_ref<detail::cron_field::day_of_week>(cex));

      detail::set_cron_days_of_month<Traits>(fields[3], detail::cron_field_ref<detail::cron_field::day_of_month>(cex));

      detail::set_cron_month<Traits>(fields[4], detail::cron_field_ref<detail::cron_field::month>(cex));

	  detail::set_cron_year<Traits>(fields[6], detail::cron_field_ref<detail::cron_field::year>(cex));
	
      return cex;
   }

   template <typename Traits = cron_standard_traits>
   static std::tm cron_next(cronexpr const & cex, std::tm date)
   {
      time_t original = utils::tm_to_time(date);
      if (INVALID_TIME == original) return {};

      if (!detail::find_next<Traits>(cex, date, date.tm_year))
         return {};

      time_t calculated = utils::tm_to_time(date);
      if (INVALID_TIME == calculated) return {};

      if (calculated == original)
      {
         add_to_field(date, detail::cron_field::second, 1);
         if (!detail::find_next<Traits>(cex, date, date.tm_year))
            return {};
      }

      return date;
   }

   template <typename Traits = cron_standard_traits>
   static std::time_t cron_next(cronexpr const & cex, std::time_t const & date)
   {
      std::tm val;
      std::tm* dt = utils::time_to_tm(&date, &val);
      if (dt == nullptr) return INVALID_TIME;

      time_t original = utils::tm_to_time(*dt);
      if (INVALID_TIME == original) return INVALID_TIME;

      if(!detail::find_next<Traits>(cex, *dt, dt->tm_year))
         return INVALID_TIME;

      time_t calculated = utils::tm_to_time(*dt);
      if (INVALID_TIME == calculated) return calculated;

      if (calculated == original)
      {
         add_to_field(*dt, detail::cron_field::second, 1);
         if(!detail::find_next<Traits>(cex, *dt, dt->tm_year))
            return INVALID_TIME;
      }

      return utils::tm_to_time(*dt);
   }   
}