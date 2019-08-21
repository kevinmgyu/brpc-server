// generated by ./ddl2cpp create_seed.sql ../sqlpp_keno_seed brpc_sqlpp
#ifndef BRPC_SQLPP_SQLPP_KENO_SEED_H
#define BRPC_SQLPP_SQLPP_KENO_SEED_H

#include <sqlpp11/table.h>
#include <sqlpp11/data_types.h>
#include <sqlpp11/char_sequence.h>

namespace brpc_sqlpp
{
  namespace KenoSeed_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint_unsigned, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update>;
    };
    struct Time
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "time";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T time;
            T& operator()() { return time; }
            const T& operator()() const { return time; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::require_insert>;
    };
    struct StartBlockNum
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "start_block_num";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T startBlockNum;
            T& operator()() { return startBlockNum; }
            const T& operator()() const { return startBlockNum; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint, sqlpp::tag::require_insert>;
    };
    struct EndBlockNum
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "end_block_num";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T endBlockNum;
            T& operator()() { return endBlockNum; }
            const T& operator()() const { return endBlockNum; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint, sqlpp::tag::require_insert>;
    };
    struct ServerSeedHash
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "server_seed_hash";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T serverSeedHash;
            T& operator()() { return serverSeedHash; }
            const T& operator()() const { return serverSeedHash; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Seed
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "seed";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T seed;
            T& operator()() { return seed; }
            const T& operator()() const { return seed; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct SeedHashTransaction
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "seed_hash_transaction";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T seedHashTransaction;
            T& operator()() { return seedHashTransaction; }
            const T& operator()() const { return seedHashTransaction; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
  } // namespace KenoSeed_

  struct KenoSeed: sqlpp::table_t<KenoSeed,
               KenoSeed_::Id,
               KenoSeed_::Time,
               KenoSeed_::StartBlockNum,
               KenoSeed_::EndBlockNum,
               KenoSeed_::ServerSeedHash,
               KenoSeed_::Seed,
               KenoSeed_::SeedHashTransaction>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "keno_seed";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T kenoSeed;
        T& operator()() { return kenoSeed; }
        const T& operator()() const { return kenoSeed; }
      };
    };
  };
  namespace KenoSeedPublish_
  {
    struct StartBlockNum
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "start_block_num";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T startBlockNum;
            T& operator()() { return startBlockNum; }
            const T& operator()() const { return startBlockNum; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint_unsigned, sqlpp::tag::require_insert>;
    };
    struct Time
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "time";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T time;
            T& operator()() { return time; }
            const T& operator()() const { return time; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::floating_point, sqlpp::tag::require_insert>;
    };
    struct EndBlockNum
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "end_block_num";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T endBlockNum;
            T& operator()() { return endBlockNum; }
            const T& operator()() const { return endBlockNum; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::bigint, sqlpp::tag::require_insert>;
    };
    struct ServerSeedHash
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "server_seed_hash";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T serverSeedHash;
            T& operator()() { return serverSeedHash; }
            const T& operator()() const { return serverSeedHash; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct Seed
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "seed";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T seed;
            T& operator()() { return seed; }
            const T& operator()() const { return seed; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct SeedHashTransaction
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "seed_hash_transaction";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T seedHashTransaction;
            T& operator()() { return seedHashTransaction; }
            const T& operator()() const { return seedHashTransaction; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
    struct SeedTransaction
    {
      struct _alias_t
      {
        static constexpr const char _literal[] =  "seed_transaction";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template<typename T>
        struct _member_t
          {
            T seedTransaction;
            T& operator()() { return seedTransaction; }
            const T& operator()() const { return seedTransaction; }
          };
      };
      using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::require_insert>;
    };
  } // namespace KenoSeedPublish_

  struct KenoSeedPublish: sqlpp::table_t<KenoSeedPublish,
               KenoSeedPublish_::StartBlockNum,
               KenoSeedPublish_::Time,
               KenoSeedPublish_::EndBlockNum,
               KenoSeedPublish_::ServerSeedHash,
               KenoSeedPublish_::Seed,
               KenoSeedPublish_::SeedHashTransaction,
               KenoSeedPublish_::SeedTransaction>
  {
    struct _alias_t
    {
      static constexpr const char _literal[] =  "keno_seed_publish";
      using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
      template<typename T>
      struct _member_t
      {
        T kenoSeedPublish;
        T& operator()() { return kenoSeedPublish; }
        const T& operator()() const { return kenoSeedPublish; }
      };
    };
  };
} // namespace brpc_sqlpp
#endif
