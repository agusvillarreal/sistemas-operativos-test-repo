#!/bin/bash
# Always start with shebang to specify interpreter

# Script name:
# Description:
# Author
# Date: YYYY-MM-DD
# Version: 1.0 -> 1.2 -> 1.6 etc...
#

read -p "Enter environment (dev/staging/prod): " env

case $env in
    dev|development)
        echo "Development environment selected"
        config_file="dev.conf"
        ;;
    staging|stage)
        echo "Staging environment selected"
        config_file="staging.conf"
        ;;
    prod|production)
        echo "Production environment selected"
        config_file="prod.conf"
        ;;
    *)
        echo "Invalid environment: $env"
        exit 1
        ;;
esac